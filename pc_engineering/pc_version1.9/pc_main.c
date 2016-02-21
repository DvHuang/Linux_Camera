#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include "main.h"



unsigned char DataBuffer[Frame_size];


int main(int argc,char* argv[])

{
   tcpinit();
   screen_info();
   
   while(1)
   {
     serrecv();
     code(DataBuffer,Frame_size);
     display();
     usleep(200000);
   }
   
   displayrelease();	
   tcprelease();
  


}
