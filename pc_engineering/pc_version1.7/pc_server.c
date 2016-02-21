#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<error.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>


#include "main.h"
char buff[65535]; 
#define UCHAR unsigned char
//#define Frame_size 320*240*2

int Ret;
int i=0;
int j=0;
int m=0;
int NewConnection;//新的连接 套结字
extern unsigned  char  DataBuffer[];

//void set_timer()
//{
//        struct itimerval itv, oldtv;
//        itv.it_interval.tv_sec = 0;
//        itv.it_interval.tv_usec = 50000;
//        itv.it_value.tv_sec = 0;
//        itv.it_value.tv_usec = 50000;
//
//        setitimer(ITIMER_REAL, &itv, &oldtv);
//}
//
//void sigalrm_handler(int sig)
//{
////        count++;
////        printf("timer signal.. %d\n", count);
//	j++;
//	printf("\n(%d)send%d:  ",j,buff[j]);
//	Ret = send(NewConnection, buff, 8192, 0);
//}
   int      ListeningSocket;//监听 套结字

  
   int      ClientAddrLen;

   int      Port;//定义端口
   struct   sockaddr_in    ClientAddr;


void  tcpinit()
{
   int i;

   int      Ret;

   struct   sockaddr_in    ServerAddr;
  

   FILE *File;
   FILE* fp;
   
   ///////////////read ini
  fp = fopen("config.ini", "r");
    if (fp == NULL)
    {
        printf("open file error！\n");
        fclose(fp);
        exit(0);
    }
    while (fscanf(fp, "%s\n", buff) == 1)
    {
        //printf("%s\n", buff);
        switch (buff[0])
        {
        case '#':
            break;
        case 'c':
            Port = atoi(buff + 4);
            break;
        default:
            break;
        }
    }
    printf("COM=%d\n", Port);
    fclose(fp);
//////////////

   //
   memset(DataBuffer, 0, sizeof(DataBuffer));

   if((ListeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)//创建监听套结字
   {
      printf("socket failed with error \n");
      return;
   } 

   ServerAddr.sin_family = AF_INET;
   ServerAddr.sin_port = htons(Port);    
   ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
   
   // 接收缓冲区
   int nRecvBuf=Frame_size;//设置为32K
   setsockopt(ListeningSocket,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));

   if(bind(ListeningSocket, (struct   sockaddr *)&ServerAddr, sizeof(ServerAddr)) == -1)//邦定端口
   {
      printf("bind failed with error \n");
      close(ListeningSocket);
      return;
   }

   if(listen(ListeningSocket, 10) == -1)//在端口上进行监听
   {
      printf("listen failed with error \n");
      close(ListeningSocket);
      return;
   } 

   printf("We are awaiting a connection on port %d......\n", Port);
       ClientAddrLen=sizeof(struct sockaddr_in);

   
     if(( NewConnection = accept(ListeningSocket, (struct   sockaddr*) &ClientAddr, &ClientAddrLen)) == -1)//等待一个连接
     {
        printf("accept failed with error \n");
        close(ListeningSocket);
        //continue;
     }

     printf("We successfully got a connection from %s:%d.\n",inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
   
     printf("We are waiting to receive data...\n");
	 
   return;
 }
 char* serrecv()
 {
   
 
 

	
		

	 	//memset(buff, 0, Frame_size);
		//for(i=0;i<8192;i++)
	  //buff[i]=rand();
		//buff[8192]=0;

	 
	  /*File=fopen("send_server.dat","ab+");
		if(File)
		{
			fwrite(DataBuffer,8192,1,File);
		}
		fclose(File);

		signal(SIGALRM, sigalrm_handler);
		set_timer();
		*/
		while(1){
		
		 memset(DataBuffer, 0, sizeof(DataBuffer));
		 Ret = recv(NewConnection, DataBuffer, sizeof(DataBuffer), 0);
		 printf("%d\n",Ret);
         if(Ret >0)
          {
		         /*for(i=0;i<8192;i++)
				     printf("%02x ",DataBuffer[i]);
			       printf("\n");*/
		      for(m=0;m<10;m++)
                       {
                      
		       printf("(%d)recv%d\n", m,DataBuffer[m]);
                       }         
		         /*
		         File=fopen("recv_server.dat","ab+");
			       if(File)
			       {
				      fwrite(DataBuffer,8192,1,File);
			       }
			       fclose(File);
		       if(atoi(DataBuffer) == 1)//＝＝1 结束当前连接
		         {
		          printf("The connection have ckosed!\n");
			        break;
			   
                 }

                 */

	    // if(m==Frame_size)
            return ;		   
          }
		 
		
	
        printf("We are awaiting a connection on port %d......\n", Port);
     //        exit(0);

   	
}
}
