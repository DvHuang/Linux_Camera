/********************************************************************************
         *                example5.c
         *        Author:        T-bagwell
*        
         *        Compile:gcc -Wall example5.c -o example5
*******************************************************************************/

		#include <stdio.h>
        #include <unistd.h>
        #include <sys/types.h>
        #include <sys/stat.h>
        #include <fcntl.h>
        #include <linux/fb.h>
        #include <errno.h>
        #include <sys/mman.h>
        #include <stdlib.h>
        #include <string.h>
        #include <sys/ioctl.h>

	
        #include "main.h"


		extern unsigned char  R[];
 		extern unsigned char  G[];
 		extern unsigned char  B[];
        int print_screen(short *buf,int width,int height);
		int print_screen_ov9650(int *buf,int width,int height);
        void draw_hline(short *buffer,int x,int y,int maxy,int maxx);
        char *fb_addr;
        unsigned fb_size;
        
      
                int screen_fbd=0;
                struct fb_fix_screeninfo fb_fix;
                struct fb_var_screeninfo fb_var;
                char *env=NULL;
                short *color_white;
		void screen_info(){
                if(!(env = getenv("FRAMEBUFFER"))) 
                {
                        env = "/dev/fb0";
                }
        
                screen_fbd = open(env, O_RDWR);
                
                if(screen_fbd < 0) 
                {
                        return 0;
                }
                
                if(ioctl(screen_fbd, FBIOGET_FSCREENINFO, &fb_fix) == -1) 
                {
                        close(screen_fbd);
                        return 0;
                }        
        
                if(ioctl(screen_fbd, FBIOGET_VSCREENINFO, &fb_var) == -1) 
                {
                        close(screen_fbd);
                        return 0;
                }
                
                fb_size = fb_var.yres * fb_fix.line_length; 


				 fprintf(stdout,"<------------screen infomation------------->\n");  
    			 fprintf(stdout,"id=%s\n",fb_fix.id);  
   				 fprintf(stdout,"x=%d\n",fb_var.xres);  
   				 fprintf(stdout,"y=%d\n",fb_var.yres);  
   				 fprintf(stdout,"bpp=%d\n",fb_var.bits_per_pixel);  
   				 fprintf(stdout,"redoffset=%d,redlength=%d,msb_right=%d\n",  
           	 fb_var.red.offset,fb_var.red.length,fb_var.red.msb_right);  
   		 fprintf(stdout,"greenoffset=%d,greenlength=%d,msb_right=%d\n",  
           	 fb_var.green.offset,fb_var.green.length,fb_var.green.msb_right);  
  		 fprintf(stdout,"blueoffset=%d,bluelength=%d,msb_right=%d\n",  
            	 fb_var.blue.offset,fb_var.blue.length,fb_var.blue.msb_right);  
   				 fprintf(stdout,"screensize=%d\n",fb_size);  

	}
        int display()
        {
                 fb_addr = (char *)mmap(NULL, fb_size, PROT_READ|PROT_WRITE,MAP_SHARED, screen_fbd, 0);
                 printf("%d",sizeof(&fb_addr));
				/* for(k=0;k<320;k++){
				 	
				 	
	   	             if(x<1280){   
	   	                
				       		 *(fb_addr+k*1280+x)=0xff87CEEB;
							
						    
                       	     x=x+1;
							 if(x==1279)x=0;
	   	               }
 
                 }
                */
                
                  
                int num[100];
               // memcpy((int*)num,(int*)fb_addr,400);
                int num_i;
              //  for(num_i=0;num_i<100;num_i++)
              //  printf("%x\n",num[num_i]);
                 
					
               
                 

			//	color_white=(char *)malloc(fb_var.yres * fb_fix.line_length);//memory allocation
              //  draw_hline(color_white,0,30,fb_var.yres,fb_var.xres);
               // print_screen(color_white,fb_var.xres,fb_var.yres);
			//print_screen_ov9650(codebuff,fb_var.xres,fb_var.yres);
                          print_screen_ov9650(codebuff,1280,320);
        /*        for(num_i=0;num_i<3;num_i++)
           printf("codebuff_data %x\n",codebuff[num_i]);
 		        memcpy((int*)num,(int*)fb_addr,40);
                for(num_i=0;num_i<3;num_i++)
           printf("fb_addr_foward  %x\n",num[num_i]);
           	
             	memcpy((int*)num,(int*)fb_addr+1040,40);
                for(num_i=0;num_i<3;num_i++)
           printf("fb_addr_data  ovpicture%x\n",num[num_i]);
                if (!fb_addr)
                exit(1);
                free(color_white);        
                return 0;*/
        }
        
        /*********************************************************
         * function:        print_screen
         *args
         *        buf:        the address of the buffer
         *        width:        the width of the framebuffer
         *         height:        the height of the framebuffer
         ********************************************************/
        int print_screen(short *buf,int width,int height)
        {
                        short *t_data = (short *)buf;
                        short *t_fb_addr = (short *)fb_addr;
                        int bytew= width<<1; 
                        while(--height >= 0)         
                        {                
                                memcpy(t_fb_addr,t_data,bytew);                                
                                t_fb_addr += width;                
                                t_data += width;        
                        }
                        return 0;
        } 


		int print_screen_ov9650(int *buf,int width,int height)
        {               int width_ov=240;
                        int *t_data = (int *)buf;
			int t_data2[1040]={0};
            int t_data2_i;
            for(t_data2_i=0;t_data2_i<1040;t_data2_i++)
               t_data2[t_data2_i]=0x9acd32;


	    int t_data3[240]={0};
            int t_data3_i;
            for(t_data3_i=0;t_data3_i<240;t_data3_i++)
               t_data3[t_data3_i]=0xff83fa;


			
                   int *t_fb_addr = (int *)fb_addr;
                   int bytew=( width_ov)<<2;
		   int bytew2=( width-width_ov)<<2;
			//printf("*buf %x\n",*buf);
                    //    printf("buf %x\n",buf);
                     //   printf("*t_data %x\n",*t_data);
                     //   printf("t_data %x\n",t_data);
					//	printf("*t_fb_addr %x\n",*t_fb_addr);
                     //   printf("t_fb_addr %x\n",t_fb_addr);
                        while(--height >= 0)         
                        {                
                                memcpy(t_fb_addr,t_data2,bytew2);
				
				memcpy(t_fb_addr+( width-width_ov-width_ov),t_data3,bytew);
				
				memcpy(t_fb_addr+( width-width_ov),t_data,bytew);
                                
				t_fb_addr += width;                
                          
				t_data += width_ov;
                        
                        }
                       // printf("*t_data mmcpylater%x\n",*t_data);
                       // printf("t_data memcpylater %x\n",t_data);
                return 0;
        }
/*********************************************************
         * function:        draw_hline        
         *args
         *        line_buffer:        the address of the buffer
         *        x:                the x of the line start point on the framebuffer
         *         y:                the y of the line start point on the framebuffer
         *         maxy:                the height of the framebuffer
         *         maxx:                the width of the framebuffer
         ********************************************************/
        void draw_hline(short *line_buffer,int x,int y,int maxy,int maxx)
        {
                int i=0,j=0;
        
                for(j=0;j<maxy;j++)
                {
                        if(j==y)
                        {
                                for(i=0;i<maxx;i++)
                                {
                                        *line_buffer++=0xf800;
                                }
                        }
                        else
                        {
                                for(i=0;i<maxx;i++)
                                {
                                        *line_buffer++=0xffff;
                                }
                        }
                }
                return ;
        }
