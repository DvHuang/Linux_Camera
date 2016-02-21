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



#define c_RGB565_MASK_RED 0xF8
#define ch_RGB565_MASK_GREEN 0x07
#define cl_RGB565_MASK_GREEN 0xD0
#define c_RGB565_MASK_BLUE 0x1F


extern unsigned char DataBuffer[];
//extern unsigned int codebuff[];




int* code(char* DataBuffer,int len)
{   
  
	
    int x=0,y=0,i=0,k=0,fi=0;
	unsigned char R,G,B;
    
	
	
	//	FILE *shuju_fd,*shuju2_fd;
    // char image2[20]="nihaoawocaonima";
    // freopen("/opt/FriendlyARM/mini2440/rootfs_qtopia_qt4/camerashuju","w",stdout);
    // freopen("/camera_data","w",stdout);
    printf("step into the code file\n");
 
     
      for(k=0;k<320;k++)
        {	
	  for(x=0;x<240;x++)
            {					 
            unsigned short wPix16;
			
			wPix16=DataBuffer[i]|DataBuffer[i+1]<<8;
			
 			/// @brief 多字节位移要注意大端小端问题，x86默认小端
        
			R = ((wPix16 & RGB565_MASK_RED) >> 11) * (256 / 32); ///< 取值范围0-31，线性放大
         	G = ((wPix16 & RGB565_MASK_GREEN) >> 5) * (256 / 64);///< 取值范围0-63，线性放大
         	B = (wPix16 & RGB565_MASK_BLUE) * (256 / 32);            ///< 取值范围0-31，线性放大
         
			//codebuff[fi]=(R[fi]<<8)|(G[fi]<<16)|(B[fi]<<24);
        	
          codebuff[fi].rgbR=R;
           codebuff[fi].rgbR=G;
			codebuff[fi].rgbR=B;
			
            //codebuff[fi]=((((DataBuffer[i]|DataBuffer[i+1]<<8) & B_RGB565_MASK_RED) << 8)|(((DataBuffer[i]|DataBuffer[i+1]<<8) & B_RGB565_MASK_GREEN) <<5)|(((DataBuffer[i]|DataBuffer[i+1]<<8) & B_RGB565_MASK_BLUE)<<3));
         
			if(fi==1|fi==5|fi==3) 
            printf("pc_code codebuff %d\n",codebuff[fi].rgbR);
            i=i+2;
			fi=fi+1;
         
             
			 
             if((k==319)&&(x==239)&&(fi==239*319)){printf("fi=239*319=%d\n",fi);
			 break;
			 }
             

			// return codebuff;
                
            }
         
        }
     
    

}

