#ifndef _MAIN_H
#define _MAIN_H

#define  Frame_size  320*240*2
#define  Frame_size8  320*240*2
#define  Frame_size_i  320*240

extern unsigned char DataBuffer[Frame_size];
extern unsigned int codebuff[Frame_size_i];

extern void tcpinit();
extern char* serrecv();
extern int* code();
extern int display();

extern void displayrelease();
extern void tcprelease();



#endif
