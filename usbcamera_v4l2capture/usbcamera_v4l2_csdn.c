#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <getopt.h> 

#include <fcntl.h> 
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h> 
#include <linux/videodev2.h>

#define CLEAR(x) memset (&(x), 0, sizeof (x))

struct buffer {
        void * start;
        size_t length;
};

static char * dev_name = "/dev/video0";//摄像头设备名
static int fd = -1;
struct buffer * buffers = NULL;
static unsigned int n_buffers = 0;

FILE *file_fd;
static unsigned long file_length;
static unsigned char *file_name;
//////////////////////////////////////////////////////
//获取一帧数据
//////////////////////////////////////////////////////
static int read_frame (void)
{
    struct v4l2_buffer buf;
    unsigned int i;

    CLEAR (buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;
/*8.出队列以取得已采集数据的帧缓冲，取得原始采集数据。VIDIOC_DQBUF*/
    //  size=buffers[buf.index].length;
    // printf("buffers.length=%d\n",buffers[buf.index].length);   
 int ff = ioctl (fd, VIDIOC_DQBUF, &buf);
    if(ff<0)
        printf("failture\n"); //出列采集的帧缓冲

    assert (buf.index < n_buffers);
       printf ("buf.index dq is %d,\n",buf.index);

    fwrite(buffers[buf.index].start, buffers[buf.index].length, 1, file_fd); //将其写入文件中
/*9.将缓冲重新入队列尾,这样可以循环采集。VIDIOC_QBUF*/ 
    ff=ioctl (fd, VIDIOC_QBUF, &buf); //再将其入列
    if(ff<0)//把数据从缓存中读取出来
        printf("failture VIDIOC_QBUF\n");
    return 1;
}

int main (int argc,char ** argv)
{
    struct v4l2_capability cap;
    struct v4l2_format fmt;
    unsigned int i;
    enum v4l2_buf_type type;

    file_fd = fopen("test-mmap.jpg", "w");//图片文件名
/*1.打开设备文件。 int fd=open(”/dev/video0″,O_RDWR);*********/
    fd = open (dev_name, O_RDWR /* required */, 0);//打开设备

/*2.取得设备的capability，看看设备具有什么功能，比如是否具有视频输入,或者音频输入输出等。VIDIOC_QUERYCAP,struct v4l2_capability*/
    int ff=ioctl (fd, VIDIOC_QUERYCAP, &cap);//获取摄像头参数
    if(ff<0)
        printf("failture VIDIOC_QUERYCAP\n");

/*3.设置视频的制式和帧格式，制式包括PAL，NTSC，帧的格式个包括宽度和高度等。*/
        struct v4l2_fmtdesc fmt1;
        int ret;
        memset(&fmt1, 0, sizeof(fmt1));
        fmt1.index = 0;
        fmt1.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //获取当前驱动支持的视频格式
        while ((ret = ioctl(fd, VIDIOC_ENUM_FMT, &fmt1)) == 0)
        {
              fmt1.index++;
              printf("{ pixelformat = '%c%c%c%c', description = '%s' }\n",fmt1.pixelformat & 0xFF, (fmt1.pixelformat >> 8) & 0xFF,(fmt1.pixelformat >> 16) & 0xFF, (fmt1.pixelformat >> 24) & 0xFF,fmt1.description);
        }
    //帧的格式，比如宽度，高度等
    CLEAR (fmt);
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //数据流类型，必须永远是V4L2_BUF_TYPE_VIDEO_CAPTURE
    fmt.fmt.pix.width = 320;//宽，必须是16的倍数
    fmt.fmt.pix.height = 240;////高，必须是16的倍数
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_JPEG;//视频数据存储类型//V4L2_PIX_FMT_YUYV;//V4L2_PIX_FMT_YVU420;//V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
    //设置当前驱动的频捕获格式 
    ff = ioctl (fd, VIDIOC_S_FMT, &fmt); 
    if(ff<0)
        printf("failture VIDIOC_S_FMT\n");
    //计算图片大小
    file_length = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height; 

/*4.向驱动申请帧缓冲，一般不超过5个。struct v4l2_requestbuffers*/
    struct v4l2_requestbuffers req;
    CLEAR (req);
    req.count = 1;//缓存数量，也就是说在缓存队列里保持多少张照片
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;//或V4L2_MEMORY_USERPTR

    ff = ioctl (fd, VIDIOC_REQBUFS, &req); //申请缓冲，count是申请的数量
    if(ff<0)
        printf("failture VIDIOC_REQBUFS\n");
    if (req.count < 1)
        printf("Insufficient buffer memory\n");

    buffers = (struct buffer*)calloc (req.count, sizeof (*buffers));//内存中建立对应空间
/*5.将申请到的帧缓冲映射到用户空间，这样就可以直接操作采集到的帧了，而不必去复制。mmap*/
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
    { 
        struct v4l2_buffer buf; //驱动中的一帧
        CLEAR (buf);
           buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
           buf.memory = V4L2_MEMORY_MMAP;
           buf.index = n_buffers;
        //把VIDIOC_REQBUFS中分配的数据缓存转换成物理地址 
           if (-1 == ioctl (fd, VIDIOC_QUERYBUF, &buf)) //映射用户空间
                printf ("VIDIOC_QUERYBUF error\n");

           buffers[n_buffers].length = buf.length;
           buffers[n_buffers].start = mmap (NULL /* start anywhere */,buf.length,PROT_READ | PROT_WRITE /* required */,MAP_SHARED /* recommended */,fd, buf.m.offset);//通过mmap建立映射关系,返回映射区的起始地址

           if (MAP_FAILED == buffers[n_buffers].start)
            printf ("mmap failed\n");
        }
/*6.将申请到的帧缓冲全部入队列，以便存放采集到的数据.VIDIOC_QBUF,struct v4l2_buffer*/
    for (i = 0; i < n_buffers; ++i)
    {
           struct v4l2_buffer buf;
           CLEAR (buf);

           buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
         buf.memory = V4L2_MEMORY_MMAP;
           buf.index = i;
            //把数据从缓存中读取出来 
           if (-1 == ioctl (fd, VIDIOC_QBUF, &buf))//申请到的缓冲进入列队
                printf ("VIDIOC_QBUF failed\n");
    }
                
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
/*7.开始视频的采集。VIDIOC_STREAMON*/
    if (-1 == ioctl (fd, VIDIOC_STREAMON, &type)) //开始捕捉图像数据
           printf ("VIDIOC_STREAMON failed\n");

    for (;;) //这一段涉及到异步IO
    {
           fd_set fds;
           struct timeval tv;
           int r;

           FD_ZERO (&fds);//将指定的?件描述符集清空
           FD_SET (fd, &fds);//在文件描述符集合中增鍔????个新的文件描述符

           /* Timeout. */
           tv.tv_sec = 2;
           tv.tv_usec = 0;

           r = select (fd + 1, &fds, NULL, NULL, &tv);//判断是否可读（即摄像头是否准备好），tv是定时

           if (-1 == r) 
        {
                if (EINTR == errno)
                     continue;
                printf ("select err\n");
                }
           if (0 == r) {
                fprintf (stderr, "select timeout\n");
                exit (EXIT_FAILURE);
                }

           if (read_frame ())//如果可读，执行read_frame ()函数，并跳出循环
               break;
    }

    unmap:
    for (i = 0; i < n_buffers; ++i)
       if (-1 == munmap (buffers->start, buffers->length))
            printf ("munmap error");
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE; 
/*10.停止视频的采集。VIDIOC_STREAMOFF*/
        if (-1 == ioctl(fd, VIDIOC_STREAMOFF, &type)) 
                printf("VIDIOC_STREAMOFF");
/*11.关闭视频设备。close(fd);*/
    close (fd);
    fclose (file_fd);
    exit (EXIT_SUCCESS);
    return 0;
}

