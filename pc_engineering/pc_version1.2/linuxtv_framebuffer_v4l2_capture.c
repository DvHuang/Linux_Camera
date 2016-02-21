/* 
*  This program can be used and distributed without restrictions. 
*/  
 
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <fcntl.h>              /* low-level i/o */  
#include <unistd.h>  
#include <errno.h>  
#include <assert.h>  
#include <sys/stat.h>  
#include <sys/types.h>  
#include <sys/time.h>  
#include <sys/mman.h>  
#include <sys/ioctl.h>  
#include <stdbool.h>  
 
#include <linux/videodev2.h>  
#include <linux/fb.h>  
 
#define CLEAR(x) memset(&(x), 0, sizeof(x))  
  
struct buffer {  
        void   *start;  
        size_t  length;  
};  
  
static char *dev_name1;  
static char *dev_name2;  
static int  fd1 = -1;  
static int  fd2 = -1;  
  
struct buffer          *buffers;  
static unsigned int     n_buffers;  
static char *fb_buffer;  
static unsigned long screensize;  
  
static char *yuv_buffer;  
static char *rgb_buffer;  
  
  
static int              force_format=false;  
static int              frame_count = 1000;  
  
static void errno_exit(const char *s)  
{  
        fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));  
        exit(EXIT_FAILURE);  
}  
  
static int xioctl(int fh, int request, void *arg)  
{  
        int r;  
  
        do {  
                r = ioctl(fh, request, arg);  
        } while (-1 == r && EINTR == errno);  
  
        return r;  
}  
/* 
static void process_image(const void *p, int size) 
{ 
    unsigned short *fb; 
    unsigned short *rgb; 
    unsigned long height = 480; 
    if(size>screensize) 
        size=screensize; 
 
    //move 
    memcpy(yuv_buffer,p,size); 
 
    //proc 
    memcpy(rgb_buffer,yuv_buffer,size); 
 
    //move 
    memcpy(fb_buffer,yuv_buffer,size); 
     
 
    //move 
    rgb=(unsigned short*)rgb_buffer; 
    fb=(unsigned short*)fb_buffer; 
    while(height--) 
    { 
        memcpy(fb,rgb,1280); 
        fb+=640; 
        rgb+=640;    
    } 
 
     
    fflush(stderr); 
    fprintf(stderr, "."); 
    fflush(stdout); 
} 
*/  
static  void YUV2RGB(unsigned char Y,unsigned char U,unsigned char V,  
                    unsigned char *R,unsigned char *G,unsigned char *B)  
/*static  void YUV2RGB(int Y,int U,int V, 
                    int *R,int *G,int *B)*/                   
{  
/* 
    *R=Y+1.4075*(V-128); 
    *G=Y-0.3455*(U-128)-0.7169*(V-128); 
    *B=Y+1.779*(U-128); 
*/  
  
    *R=Y+(V-128)+((V-128)*103>>8);  
    *G=Y-((U-128)*88>>8)-((V-128)*183>>8);  
    *B=Y+(U-128)+((U-128)*198>>8);  
}  
  
static void process_image(const unsigned long *from, int size)  
{  
    unsigned long *to =(unsigned long*)fb_buffer;  
      
    unsigned char Y0;  
    unsigned char U0;  
    unsigned char Y1;  
    unsigned char V0;  
      
    unsigned char R0;  
    unsigned char G0;  
    unsigned char B0;  
    unsigned char R1;  
    unsigned char G1;  
    unsigned char B1;     
  
    size>>=2;  
    while(size--)  
    {  
        Y0=(*from & 0x000000FF)>>0;         
        U0=128;                         //white and black  
        //U0=(*from & 0x0000FF00)>>8; //colorful  
        Y1=(*from & 0x00FF0000)>>16;        
        V0=128;                         //white and blcak  
        //V0=(*from & 0xFF000000)>>24;    //colorful        
        YUV2RGB(Y0,U0,V0,&R0,&G0,&B0);  
        YUV2RGB(Y1,U0,V0,&R1,&G1,&B1);  
  
        *to = (R0&0x1F)<<11 | (G0&0x3F)<<5 |(B0&0x1F)<<0;  
        *to |= ((R1&0x1F)<<11 | (G1&0x3F)<<5 |(B1&0x1F)<<0)<<16;          
          
        from++;  
        to++;  
    }  
}  
      
static int read_frame(void)  
{  
    struct v4l2_buffer buf;  
  
    CLEAR(buf);  
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    buf.memory = V4L2_MEMORY_MMAP;  
    if (-1 == xioctl(fd1, VIDIOC_DQBUF, &buf))  
    {  
        switch (errno)   
        {  
            case EAGAIN:  
                return 0;  
            case EIO:  
            default:  
                errno_exit("VIDIOC_DQBUF");  
        }  
    }  
  
    assert(buf.index < n_buffers);  
      
    process_image(buffers[buf.index].start, buf.bytesused);  
  
    if (-1 == xioctl(fd1, VIDIOC_QBUF, &buf))  
        errno_exit("VIDIOC_QBUF");  
  
    return 1;  
}  
  
static void mainloop(void)  
{  
        unsigned int count;  
  
        count = frame_count;  
        while (count-- > 0) {  
                for (;;) {  
                        fd_set fds;  
                        struct timeval tv;  
                        int r;  
  
                        FD_ZERO(&fds);  
                        FD_SET(fd1, &fds);  
  
                        /* Timeout. */  
                        tv.tv_sec = 2;  
                        tv.tv_usec = 0;  
  
                        r = select(fd1 + 1, &fds, NULL, NULL, &tv);  
  
                        if (-1 == r) {  
                                if (EINTR == errno)  
                                        continue;  
                                errno_exit("select");  
                        }  
  
                        if (0 == r) {  
                                fprintf(stderr, "select timeout\n");  
                                exit(EXIT_FAILURE);  
                        }  
  
                        if (read_frame())  
                                break;  
                        /* EAGAIN - continue select loop. */  
                }  
        }  
}  
  
static void stop_capturing(void)  
{  
    enum v4l2_buf_type type;  
      
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    if (-1 == xioctl(fd1, VIDIOC_STREAMOFF, &type))  
        errno_exit("VIDIOC_STREAMOFF");  
}  
  
static void start_capturing(void)  
{  
    unsigned int i;  
    enum v4l2_buf_type type;  
    struct v4l2_buffer buf;  
          
    for (i = 0; i < n_buffers; ++i)   
    {         
        CLEAR(buf);  
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
        buf.memory = V4L2_MEMORY_MMAP;  
        buf.index = i;  
        if (-1 == xioctl(fd1, VIDIOC_QBUF, &buf))  
            errno_exit("VIDIOC_QBUF");  
    }  
      
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    if (-1 == xioctl(fd1, VIDIOC_STREAMON, &type))  
        errno_exit("VIDIOC_STREAMON");  
  
}  
  
static void uninit_device(void)  
{  
        unsigned int i;  
  
    for (i = 0; i < n_buffers; ++i)  
        if (-1 == munmap(buffers[i].start, buffers[i].length))  
            errno_exit("munmap");  
          
    if(-1 == munmap(fb_buffer,screensize))  
        errno_exit("munmap");  
      
    free(buffers);  
    free(rgb_buffer);  
    free(yuv_buffer);  
}  
  
static void init_mmap(void)  
{  
    struct v4l2_requestbuffers req;  
    struct v4l2_buffer buf;  
  
    //VIDIOC_REQBUFS  
    CLEAR(req);  
    req.count = 4;  
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    req.memory = V4L2_MEMORY_MMAP;  
    if(-1 == xioctl(fd1, VIDIOC_REQBUFS, &req))   
    {  
        if (EINVAL == errno)   
        {  
            fprintf(stderr, "%s does not support ""memory mapping\n", dev_name1);  
            exit(EXIT_FAILURE);  
        }   
        else  
            errno_exit("VIDIOC_REQBUFS");  
    }  
    if(req.count < 2)   
    {  
            fprintf(stderr, "Insufficient buffer memory on %s\n",dev_name1);  
            exit(EXIT_FAILURE);  
    }  
  
    //VIDIOC_QUERYBUF  
    buffers = calloc(req.count, sizeof(*buffers));  
    if (!buffers)   
    {  
            fprintf(stderr, "Out of memory\n");  
            exit(EXIT_FAILURE);  
    }  
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers)   
    {          
        CLEAR(buf);  
        buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
        buf.memory      = V4L2_MEMORY_MMAP;  
        buf.index       = n_buffers;  
        if (-1 == xioctl(fd1, VIDIOC_QUERYBUF, &buf))  
            errno_exit("VIDIOC_QUERYBUF");  
        buffers[n_buffers].length = buf.length;  
        buffers[n_buffers].start =mmap(NULL /* start anywhere */,buf.length,  
                                PROT_READ | PROT_WRITE /* required */,  
                                MAP_SHARED /* recommended */,  
                                fd1, buf.m.offset);  
        if (MAP_FAILED == buffers[n_buffers].start)  
        {  
            errno_exit("mmap");  
        }  
    }//now n_buffers is 4  
  
    //fbmmap  
    fb_buffer=(char*)mmap(NULL,screensize,PROT_READ | PROT_WRITE,MAP_SHARED,fd2,0);  
    if(MAP_FAILED == fb_buffer)  
    {  
        errno_exit("mmap");  
    }  
      
    //malloc the yuv_buffer  
    yuv_buffer = (char*)malloc(screensize);  
    if (!yuv_buffer)   
    {  
            fprintf(stderr, "Out of memory\n");  
            exit(EXIT_FAILURE);  
    }  
      
    //malloc the rgb_buffer  
    rgb_buffer = (char*)malloc(screensize);  
    if (!rgb_buffer)   
    {  
            fprintf(stderr, "Out of memory\n");  
            exit(EXIT_FAILURE);  
    }     
}  
  
static void init_device(void)  
{  
    struct v4l2_capability cap;  
    struct v4l2_format fmt;  
    struct fb_fix_screeninfo finfo;  
    struct fb_var_screeninfo vinfo;  
  
    CLEAR(cap);  
    if (-1 == xioctl(fd1, VIDIOC_QUERYCAP, &cap))  
    {  
        if (EINVAL == errno)   
        {  
            fprintf(stderr, "%s is no V4L2 device\n",dev_name1);  
            exit(EXIT_FAILURE);  
        }   
        else   
            errno_exit("VIDIOC_QUERYCAP");  
    }  
    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))   
    {  
        fprintf(stderr, "%s is no video capture device\n",dev_name1);  
        exit(EXIT_FAILURE);  
    }  
    if (!(cap.capabilities & V4L2_CAP_STREAMING))   
    {  
        fprintf(stderr, "%s does not support streaming i/o\n",dev_name1);  
        exit(EXIT_FAILURE);  
    }  
  
    CLEAR(fmt);  
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  
    if (force_format)   
    {  
        fmt.fmt.pix.width       = 640;  
        fmt.fmt.pix.height      = 480;  
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;  
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;  
        if (-1 == xioctl(fd1, VIDIOC_S_FMT, &fmt))  
            errno_exit("VIDIOC_S_FMT");  
    }   
    else  
    {  
        if (-1 == xioctl(fd1, VIDIOC_G_FMT, &fmt))  
            errno_exit("VIDIOC_G_FMT");  
    }  
    fprintf(stdout,"<------------camera infomation------------->\n");  
    fprintf(stdout,"device driver=%s\n",cap.driver);  
    fprintf(stdout,"device name=%s\n",cap.card);  
    fprintf(stdout,"bus_infomation=%s\n",cap.bus_info);  
    fprintf(stdout,"image_width=%d\n",fmt.fmt.pix.width);  
    fprintf(stdout,"image_height=%d\n",fmt.fmt.pix.height);  
    fprintf(stdout,"pixel_format=%d\n",fmt.fmt.pix.pixelformat);  
    fprintf(stdout,"\n");  
  
  
    CLEAR(finfo);  
    if(-1 == xioctl(fd2,FBIOGET_FSCREENINFO,&finfo))  
        errno_exit("FBIOGET_FSCREENINFO");  
  
    CLEAR(vinfo);  
    if(-1 == xioctl(fd2,FBIOGET_VSCREENINFO,&vinfo))  
        errno_exit("FBIOGET_VSCREENINFO");  
    screensize = vinfo.xres*vinfo.yres*vinfo.bits_per_pixel/8;  
  
    fprintf(stdout,"<------------screen infomation------------->\n");  
    fprintf(stdout,"id=%s\n",finfo.id);  
    fprintf(stdout,"x=%d\n",vinfo.xres);  
    fprintf(stdout,"y=%d\n",vinfo.yres);  
    fprintf(stdout,"bpp=%d\n",vinfo.bits_per_pixel);  
    fprintf(stdout,"redoffset=%d,redlength=%d,msb_right=%d\n",  
            vinfo.red.offset,vinfo.red.length,vinfo.red.msb_right);  
    fprintf(stdout,"greenoffset=%d,greenlength=%d,msb_right=%d\n",  
            vinfo.green.offset,vinfo.green.length,vinfo.green.msb_right);  
    fprintf(stdout,"blueoffset=%d,bluelength=%d,msb_right=%d\n",  
            vinfo.blue.offset,vinfo.blue.length,vinfo.blue.msb_right);  
    fprintf(stdout,"screensize=%d\n",screensize);  
      
    init_mmap();  
}  
  
static void close_device(void)  
{  
    if (-1 == close(fd1))  
        errno_exit("close");  
    fd1 = -1;  
      
    if (-1 == close(fd2))  
        errno_exit("close");  
    fd2 = -1;  
}  
  
static void open_device(void)  
{  
  
    fd1 = open(dev_name1, O_RDWR /* required */ | O_NONBLOCK, 0);  
    if (-1 == fd1)   
    {  
        fprintf(stderr, "Cannot open '%s': %d, %s\n",dev_name1, errno, strerror(errno));  
        exit(EXIT_FAILURE);  
    }  
  
    fd2 = open(dev_name2,O_RDWR,0);  
    if(-1 == fd2)  
    {  
        fprintf(stderr, "Cannot open '%s': %d, %s\n",dev_name2, errno, strerror(errno));  
        exit(EXIT_FAILURE);       
    }     
}  
  
int main(int argc, char **argv)  
{  
    dev_name1 = "/dev/video0";  
    dev_name2 = "/dev/fb0";  
  
    open_device();  
    init_device();  
    start_capturing();  
    mainloop();  
    stop_capturing();  
    uninit_device();  
    close_device();  
    fprintf(stderr, "\n");  
    return 0;  
}  