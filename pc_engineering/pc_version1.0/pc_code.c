#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/mman.h>


#include "main.h"

#define RGB565_MASK_RED 0xF800
#define RGB565_MASK_GREEN 0x07E0
#define RGB565_MASK_BLUE 0x001F

#define B_RGB565_MASK_RED 0x0000F800
#define B_RGB565_MASK_GREEN 0x000007E0
#define B_RGB565_MASK_BLUE 0x0000001F

extern unsigned char DataBuffer[];
extern unsigned int codebuff[];

int* code(char* DataBuffer,int len)
{   
    
	
    int x,y,i=0,k,fi;
   // int R,G,B;
	
	
	//	FILE *shuju_fd,*shuju2_fd;
    // char image2[20]="nihaoawocaonima";
    // freopen("/opt/FriendlyARM/mini2440/rootfs_qtopia_qt4/camerashuju","w",stdout);
    // freopen("/camera_data","w",stdout);
    
 
     
      for(k=0;k<320;k++)
        {	
	  for(x=0;x<240;x++)
            {					 
          
              /*取rgb各个分量的值
			   R = ((DataBuffer[i]|DataBuffer[i+1]<<8) & RGB565_MASK_RED) >> 10;  // 取值范围0-31
               G = ((DataBuffer[i]|DataBuffer[i+1]<<8) & RGB565_MASK_GREEN) >> 5; // 取值范围0-63
               B = ((DataBuffer[i]|DataBuffer[i+1]<<8) & RGB565_MASK_BLUE)<<1;    // 取值范围0-31
              
               各分量值移位到目的地
               R = (((DataBuffer[i]|DataBuffer[i+1]<<8) & RGB565_MASK_RED) >> 8);  // 取值范围0-31
               G = (((DataBuffer[i]|DataBuffer[i+1]<<8) & RGB565_MASK_GREEN) >> 5); // 取值范围0-63
               B = (((DataBuffer[i]|DataBuffer[i+1]<<8) & RGB565_MASK_BLUE)<<3);    // 取值范围0-31
              */

             codebuff[fi]=(((DataBuffer[i]|DataBuffer[i+1]<<8) & B_RGB565_MASK_RED) << 8)|(((DataBuffer[i]|DataBuffer[i+1]<<8) & B_RGB565_MASK_GREEN) <<5)|(((DataBuffer[i]|DataBuffer[i+1]<<8) & B_RGB565_MASK_BLUE)<<3);
			 fi++;
             if(fi==320*240)break;
			 
             i=i+2;
			 
             if((k==319)&&(x==239)) break;

			 return codebuff;
                 
            }
        }
     
    

}

