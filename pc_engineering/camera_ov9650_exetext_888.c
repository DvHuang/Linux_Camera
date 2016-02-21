#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>


#define Frame_Size  320*240*4

int main(int argc,char **argv)
{       int cam_fd,fb_fd,rgbwant=16,delay=0;
       //	FILE *shuju_fd,*shuju2_fd;
	char image[320*240*4],image2[320*240*4];
	int x,y,i=0,k;
        short int *fb_mem;
        struct fb_var_screeninfo vinfo;
//        char image2[20]="nihaoawocaonima";
       // freopen("/opt/FriendlyARM/mini2440/rootfs_qtopia_qt4/camerashuju","w",stdout);
       // freopen("/camera_data","w",stdout);

	cam_fd = open("/dev/camera",O_RDWR);
	if(cam_fd < 0) {
		perror("camera.\n");
		return -1;
                 	}
	
   //      shuju_fd = fopen("/opt/FriendlyARM/mini2440/rootfs_qtopia_qt4/camerashuju2","r+");
   //   	if(shuju_fd  < 0) {
   //		perror("shuju_fd .\n");
   //		return -1;
   //    	}
	
   //     shuju2_fd =fopen("/camera_data","r+");
   //        if(shuju2_fd<0){
   //          perror("shuju_fd2,\n");
   //          return -1;
   //        }
        fb_fd=open("/dev/fb0",O_RDWR);
         if(fb_fd <0)
           {
              perror("fb_fd open fail,\n");
              return -1;
           }
  // screensize
  // ioctl(fb_fd,FBIOGET_VSCREENINFO,&vinfo)
  // screensize=vinfo.xres*vinfo.yres*vinfo.bits_per_pixel/8;
  //
  // read(cam_fd,image,Frame_Size);
        fb_mem=mmap(0,320*240*4,PROT_WRITE|PROT_READ,MAP_SHARED,fb_fd,0);

        if(ioctl(fb_fd,FBIOGET_VSCREENINFO,&vinfo)<0){
          printf("couldn't get console pixel format\n");
          return(-1);
         }
        printf("%d",vinfo.bits_per_pixel);
 //if(vinfo.bits_per_pixel==rgbwant){
        vinfo.bits_per_pixel=32;

        printf("%d",vinfo.bits_per_pixel);

        if(ioctl(fb_fd,FBIOPUT_VSCREENINFO,&vinfo)<0)
         {
        	perror("error\n");
          }
         printf("%d",vinfo.bits_per_pixel);
   
      // for(delay=0;delay<1000000;delay++);

       ioctl(fb_fd,FBIOGET_VSCREENINFO,&vinfo);

       printf("%d",vinfo.bits_per_pixel);
//}
 
for(y=0;y<10;y++){
 read(cam_fd,image,Frame_Size);
     for(k=0;k<320;k++)
       {	
	   	
	//	read(cam_fd,image,Frame_Size);
		//通过for循环，将image的图像，进行移位，写入fb_mem 
              //  printf("%d\n",sizeof(image));
               // for(y=0;y<1024;y++)
		// {
                        	for(x=0;x<360;x++)
	//	for (x=0;x<720;x++)	
					{
					  // write(shuju_fd,&image[i],60);
                            //               write(shuju2_fd,image[i],1);
//					  fputs(image2,shuju_fd);
                                  //        fputs(image2,shuju2_fd);
                //                       image2[i]=image[i];
                  //                     i++;
//                                           printf("%d %d ",image[i],image[i+1]);
                                        //  *(fb_mem+k*320+x)=image[i]|image[i+1]<<8|image[i+2]<<8;
                           //              *(fb_mem)=15;  

//                                         *(fb_mem+k*240+x)=image[i]|image[i+1]<<8;
                                           
                                            i+=2;
                                           if(x==239)
                                                 {
                                                        printf(";");
                                                 }
                                         // if(x==1279){write(shuju2_fd,}
                                 
					}
  
                // }
}         }

  //  fclose(shuju2_fd);
    close(cam_fd);	   
    // fclose(shuju_fd);
   // fclose(stdout);
   munmap(fb_mem,320*240*4);
   close(fb_fd);

}

