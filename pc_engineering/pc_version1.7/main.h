#ifndef _MAIN_H
#define _MAIN_H

#define  Frame_size  320*240*2
#define  Frame_size8  320*240*2
#define  Frame_size_i  320*240

extern unsigned char DataBuffer[Frame_size];

 struct tagRGBQUAD 
 	{
       unsigned char rgbB; // 蓝色分量
       unsigned char rgbG; // 绿色分量
       unsigned char rgbR; // 红色分量
       unsigned char rgbReserved; // 保留字节（用作Alpha通道或忽略）
     } codebuff[320*240];

extern void tcpinit();
extern char* serrecv();
extern int* code();
extern int display();

extern void displayrelease();
extern void tcprelease();



#endif
