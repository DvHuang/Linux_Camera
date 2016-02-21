/* include head_files*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/select.h>
#include <fcntl.h>   //low_level i/o
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>  //for videodev2.h

#include <linux/videodev2.h>

#include "capture.h"

struct buffer{
	void *  start;
	size_t  length;
};

static char *        video_name     = "/dev/video2";
static int           video_fd       = -1;
struct buffer *      buffers      = NULL;
static unsigned int  n_buffers    = 0;
static unsigned int number = 0;

void process_video_image(const void * p,unsigned int size)
{
	/*static unsigned int number = 0;
	printf("frame = %d\n",number);
	number++;
	fflush(stdout);
	*/
	int file_fd;
	int write_size;
	//struct v4l2_frmsizeenum frame_size;
	//struct v4l2_fmtdesc image_fmt;
	if ((file_fd = open("pic.yuv",O_CREAT | O_RDWR | O_APPEND,777)) == -1)
	{
		printf("create a picture file error!\n");
	}
	write_size=write(file_fd,p,size);
	printf("write_size = %d\n",write_size);
	/*image_fmt.index = 10;
	image_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(video_fd,VIDIOC_ENUM_FMT,&image_fmt))
	{
		perror("VIDIOC_ENUM_FMT:");
	}
	else
	{
		printf("image format is %s\n",image_fmt.description);
	}*/
	close(file_fd);
}

int read_video_frame (void)
{
	struct v4l2_buffer buf;
	unsigned int size;
	
	CLEAR(buf);
	
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	size = buffers[buf.index].length;
	printf("buffers.length=%d\n",buffers[buf.index].length);
	if (-1 == ioctl(video_fd,VIDIOC_DQBUF,&buf))
	{
		perror("VIDIOC_DQBUF:");
		exit(EXIT_FAILURE);
		return 0;
	}
	
	assert (buf.index < n_buffers);	
	process_video_image (buffers[buf.index].start,size);
	
	if (-1 == ioctl(video_fd,VIDIOC_QBUF,&buf))
	{
		perror("VIDIOC_QBUF:");
		exit(EXIT_FAILURE);
	}
	
	return 1;
} 

void read_video_loop(void)
{
	unsigned int count ;//
	count = 20;
	
	while(count-- > 0)
	{
		for (;;)
		{
			fd_set fds;
			struct timeval tv;
			int r;
			
			FD_ZERO(&fds);
			FD_SET(video_fd,&fds);
			
			//timeout = 2sec
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			
			r = select(video_fd+1,&fds,NULL,NULL,&tv);
			
			if (-1 == r)
			{
				if (EINTR == errno)//system call interrupt
					continue;
				perror("select:");
				exit(EXIT_FAILURE);
			}
			
			if (0 == r)
			{
				fprintf(stderr,"select timeout\n");
				exit(EXIT_FAILURE);
			}
			
			if (read_video_frame())
				break;
		}
	}

}

void start_video_capturing  (void)
{
	unsigned int i;
	enum v4l2_buf_type  type;
	
	//using mmap
	for (i = 0 ; i < n_buffers; i++)
	{
		struct v4l2_buffer buf;
		CLEAR(buf);
		
		buf.type       = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory     = V4L2_MEMORY_MMAP;
		buf.index      = i;
		
		if (-1 == ioctl(video_fd,VIDIOC_QBUF,&buf))
		{
			perror("VIDIOC_QBUF:");
			exit(EXIT_FAILURE);
		}
	
	}
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	if (-1 == ioctl(video_fd,VIDIOC_STREAMON,&type))
	{
		perror("VIDOIOC_STREAMON:");
		exit(EXIT_FAILURE);
	}
	else 
	{
		printf("%s capture is opened \n",video_name);
	}
}

void init_video_mmap(void )
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count   = 4;
	req.type    = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory  = V4L2_MEMORY_MMAP;

	if (-1 == ioctl(video_fd,VIDIOC_REQBUFS,&req))
	{
		if (errno == EINVAL)
			printf("Video capturing or mmap-streaming is not supported\n");
		else 
			perror("VIDIOC_REQBUFS");
		exit(EXIT_FAILURE);
	}

	if (req.count < 2)
	{
		fprintf(stderr ,"Insufficient buffer memory on %s\n",video_name);
		exit(EXIT_FAILURE);
	}
	
	buffers = calloc(req.count,sizeof(*buffers));
	
	if (!buffers)
	{
		fprintf(stderr,"Out of memory\n");
		exit(EXIT_FAILURE);
	}
	
	for (n_buffers = 0;n_buffers < req.count ; n_buffers++)
	{
		struct v4l2_buffer  buf;
		
		CLEAR(buf);
	
		buf.type = req.type;
		buf.type = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;
		
		if (-1 == ioctl (video_fd,VIDIOC_QUERYBUF,&buf))
		{
			perror("VIDIOC_QUERYBUF");
			exit(EXIT_FAILURE);
		}
		
		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap (NULL,buf.length,
						 PROT_READ | PROT_WRITE, 
						 MAP_SHARED,video_fd,buf.m.offset);
		if (MAP_FAILED == buffers[n_buffers].start)
		{
			fprintf(stderr,"mmap is failed \n");
			exit(EXIT_FAILURE);
		}
	}
}

void open_video_device (void)
{
	struct stat st; //store the device information

	//get the device information 
	if (-1 == stat(video_name,&st))
	{
		fprintf(stderr ," Cannot identify '%s': %d , %s\n",
			video_name,errno,strerror(errno));
		exit(EXIT_FAILURE);
	}

	// test the device is or isn't a special character file 
	// because the camera device is a charcter file on linux
	if (!S_ISCHR(st.st_mode))
	{
		fprintf(stderr,"%s is no device \n",video_name);
		exit(EXIT_FAILURE);
	}

	//open device on the read , write and nonblock mode   
	if (-1 == (video_fd = open(video_name,O_RDWR | O_NONBLOCK,0)))
	{
		fprintf(stderr,"Cannot open '%s' : '%d',%s \n",
			video_name,errno,strerror(errno));
		exit(EXIT_FAILURE);
	}
	else 
		printf("camera device is open OK ! camera name is %s \n",
			video_name);
	
}

void query_video_device (void)
{
	struct v4l2_capability     cap;

	/* It is used to identify kernel devices compatible with this specification and to abtain information about driver and hardware capabilities*/
	
	if (-1 == ioctl (video_fd ,VIDIOC_QUERYCAP,&cap))
	{
		perror("VIDIOC_QUERYCAP:");	
		exit (EXIT_FAILURE);
	}
	
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		fprintf(stderr ,"%s is no video capture device\n",video_name);
		exit(EXIT_FAILURE);
	}
	else 
	{
		printf("%s is a video capture device!\n",video_name);
	}
	
	if (!(cap.capabilities & V4L2_CAP_READWRITE))
	{
		fprintf(stderr ,"%s does not support read i/o\n",video_name);
	}
	else 
	{
		printf("%s support read i/o !\n",video_name);
	}
	
	if (!(cap.capabilities & V4L2_CAP_STREAMING))
	{
		fprintf(stderr,"%s does not support streaming i/o\n",video_name);
	}
	else
	{
		printf("%s supports streaming i/o method!\n",video_name);
	}

	printf("The driver name is %s\n",cap.driver);
	printf("The device name is %s\n",cap.card);
	printf("The device bus is %s\n",cap.bus_info);
}

void change_video_properties(void)
{
	struct v4l2_cropcap cropcap;
	struct v4l2_crop  crop;
	
	CLEAR(cropcap);
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	/*cropcap.bounds.left = 0;
	cropcap.bounds.top = 0;
	cropcap.bounds.width = 160;
	cropcap.bounds.height = 120;*/
	
	if (0 == ioctl(video_fd,VIDIOC_CROPCAP,&cropcap))
	{
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect;
		printf("cropcap.defrect.left  = %d\n",cropcap.defrect.left);
		printf("cropcap.defrect.top = %d\n",cropcap.defrect.top);
		printf("cropcap.defrect.width = %d\n",cropcap.defrect.width);
		printf("cropcap.defrect.height = %d\n",cropcap.defrect.height);
		if (-1 == ioctl(video_fd,VIDIOC_G_CROP,&crop))
		{
			perror("VIDIOC_G_CROP:");
		}
	}
	else
	{
		perror("VIDIOC_CROPCAP:");
	}
}


void set_video_format(void)
{
	struct v4l2_format fmt,set_fmt,fault_fmt;
//	unsigned int min;	
	
	struct v4l2_streamparm fault_parm,set_parm,parm;

	CLEAR(fmt);
	CLEAR(set_fmt);
	CLEAR(fault_fmt);
	
	/*******************************************/
	/******  get the fault video format ********/
	/*******************************************/
	
	fault_fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(video_fd,VIDIOC_G_FMT,&fault_fmt))
	{
		perror("VIDIOC_G_FMT");
	}	
	else 
	{
		printf("fault_pixel width = %d\n",fault_fmt.fmt.pix.width);
		printf("fault_pixel_height = %d\n",fault_fmt.fmt.pix.height);
		printf("fault_pixelformat = %x\n",fault_fmt.fmt.pix.pixelformat);
		printf("fault_sizeimage = %d\n",fault_fmt.fmt.pix.sizeimage);
	}
	/********************************************************/
	/*********  set the video capture format ****************/
	/********************************************************/

	set_fmt.type                 = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	set_fmt.fmt.pix.width        = 160;
	set_fmt.fmt.pix.height       = 120;
	set_fmt.fmt.pix.pixelformat  = V4L2_PIX_FMT_YUYV;
	set_fmt.fmt.pix.field        = V4L2_FIELD_INTERLACED;

	if (-1 == ioctl(video_fd,VIDIOC_S_FMT,&set_fmt))
		perror("VIDIOC_S_FMT:");
	
	/*********************************************************/
	/********* make sure the video format set success ********/
	/*********************************************************/
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (-1 == ioctl(video_fd,VIDIOC_G_FMT,&fmt))
	{
		perror("VIDIOC_G_FMT");
	}
	else
	{
		printf("after set pixel width = %d\n",fmt.fmt.pix.width);
		printf("after set pixel_height = %d\n",fmt.fmt.pix.height);
		printf("after set pixelformat = %x\n",fmt.fmt.pix.pixelformat);
		printf("after set sizeimage = %d\n",fmt.fmt.pix.sizeimage);
	}

	CLEAR(parm);
        CLEAR(set_parm);
        CLEAR(fault_parm);

	fault_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE ;
	fault_parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;

	if (-1 == ioctl(video_fd,VIDIOC_G_PARM,&fault_parm))
	{
		perror("VIDIOC_G_PARM");
	}
	else
	{
		printf("fault timeperframe denominator = %d\n",fault_parm.parm.capture.timeperframe.denominator);
		printf("fault timeperframe numerator = %d\n",fault_parm.parm.capture.timeperframe.numerator);
	}

	set_parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE ;
	set_parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;
	set_parm.parm.capture.timeperframe.denominator = 25;
	set_parm.parm.capture.timeperframe.numerator = 1;
        if (-1 == ioctl(video_fd,VIDIOC_S_PARM,&set_parm))
        {
                perror("VIDIOC_S_PARM");
        }

	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE ;
        parm.parm.capture.capability = V4L2_CAP_TIMEPERFRAME;

	if (-1 == ioctl(video_fd,VIDIOC_G_PARM,&parm))
        {
                perror("VIDIOC_G_PARM");
        }
        else
        {
                printf("after set timeperframe denominator = %d\n",parm.parm.capture.timeperframe.denominator);
                printf("afer set  timeperframe numerator = %d\n",parm.parm.capture.timeperframe.numerator);
        }

	/*calucate the image_size
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline*fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;*/

//	init_video_mmap();
	
}

void stop_video_capturing(void)
{
	enum v4l2_buf_type type;
	
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	
	if (-1 == ioctl(video_fd,VIDIOC_STREAMOFF,&type))
	{
		perror("VIDIOC_STREAMOFF");
		exit(EXIT_FAILURE);
	}
}

void uninit_video_device(void)
{
	unsigned int i;
	for (i = 0; i < n_buffers; i++)
	{
		if (-1 == munmap(buffers[i].start,buffers[i].length))
		{
			fprintf(stderr,"munmap failed\n");
			exit(EXIT_FAILURE);
		}
	}
	free(buffers);
}
void close_video_device (void)
{
	if (-1 == close(video_fd))
		perror("close");
	printf("close %s finished \n",video_name);
	video_fd = -1;
}


