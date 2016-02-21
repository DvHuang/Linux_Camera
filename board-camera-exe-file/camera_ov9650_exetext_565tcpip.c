#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>

#define Frame_Size  320*240*2

#define RGB565_MASK_RED 0xF800
#define RGB565_MASK_GREEN 0x07E0
#define RGB565_MASK_BLUE 0x001F

int main(int argc,char **argv)
{   
    int cam_fd,fb_fd,FLAG=0;
	char image[320*240*2];
	int x,y,i=0,k;
    short int *fb_mem,R,G,B;
	
	//	FILE *shuju_fd,*shuju2_fd;
    // char image2[20]="nihaoawocaonima";
    // freopen("/opt/FriendlyARM/mini2440/rootfs_qtopia_qt4/camerashuju","w",stdout);
    // freopen("/camera_data","w",stdout);
    




	cam_fd = open("/dev/camera",O_RDWR);
	if(cam_fd < 0) 
	 {
		perror("camera.\n");
		return -1;
         }
	
//  shuju_fd = fopen("/opt/FriendlyARM/mini2440/rootfs_qtopia_qt4/camerashuju2","r+");
//	if(shuju_fd  < 0) {
//		perror("shuju_fd .\n");
//		return -1;
//	}
	
//   shuju2_fd =fopen("/camera_data","r+");
//   if(shuju2_fd<0){
//         perror("shuju_fd2,\n");
//        return -1;
//   }
     /*fb_fd=open("/dev/fb0",O_RDWR);
      if(fb_fd <0)
        {
           perror("fb_fd open fail,\n");
           return -1;
        }
     */
   
// read(cam_fd,image,Frame_Size);
// fb_mem=mmap(0,320*240*2,PROT_WRITE|PROT_READ,MAP_SHARED,fb_fd,0);
  
//  read(cam_fd,image,Frame_Size);
   for(;;)
    {
      read(cam_fd,image,Frame_Size);
      for(k=0;k<320;k++)
        {	
	  for(x=0;x<240;x++)
            {					 
             R = ((image[i]|image[i+1]<<8) & RGB565_MASK_RED) >> 10;  // 取值范围0-31
             G = ((image[i]|image[i+1]<<8) & RGB565_MASK_GREEN) >> 5; // 取值范围0-63
             B = ((image[i]|image[i+1]<<8) & RGB565_MASK_BLUE)<<1;    // 取值范围0-31
             i=i+2;
             if(R>G+B)FLAG=1;
             else FLAG=0;
             printf("%d",FLAG);
	     if((k==319)&&(x==239)) i=0;
                 
            }
        }
     }
     close(cam_fd);	  

}

