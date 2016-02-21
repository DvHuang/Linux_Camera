#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

#include "main.h"



unsigned char DataBuffer[Frame_size];
unsigned int codebuff[Frame_size_i];

int main(int argc,char* argv[])

{
   tcpinit();
   
   while(1)
   {
     *DataBuffer=serrecv();
     *codebuff=code(DataBuffer,Frame_size);
     display(codebuff,Frame_size_i);
   }
   
   displayrelease();	
   tcprelease();
  


}
