/**************************************************
         *                example5.c
         *        Author:        T-bagwell
*        
         *        Compile:gcc -Wall example5.c -o example5
*************************************************/
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
        
        int print_screen(short *buf,int width,int height);
        void draw_hline(short *buffer,int x,int y,int maxy,int maxx);
        char *fb_addr;
        unsigned fb_size;
        
        int main(int argc, char *argv[])
        {
                int screen_fbd=0;
                struct fb_fix_screeninfo fb_fix;
                struct fb_var_screeninfo fb_var;
                char *env=NULL;
                short *color_white;

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
                fb_addr = (char *)mmap(NULL, fb_size, PROT_READ|PROT_WRITE,MAP_SHARED, screen_fbd, 0);
               color_white=(char *)malloc(fb_var.yres * fb_fix.line_length);
                draw_hline(color_white,0,30,fb_var.yres,fb_var.xres);
                print_screen(color_white,fb_var.xres,fb_var.yres);
                if (!fb_addr)
                        exit(1);
                free(color_white);        
                return 0;
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
