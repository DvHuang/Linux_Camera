#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "main.h"



unsigned char DataBuffer[Frame_size];


int main(int argc,char* argv[])

{
   tcpinit();
   
   while(1)
   {
     serrecv();
     code(DataBuffer,Frame_size);
     display();
   }
   
   displayrelease();	
   tcprelease();
  


}
