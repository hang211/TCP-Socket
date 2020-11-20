#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <sys/timeb.h>
#include <pthread.h>

#define BUFFER_SIZE 1024
#define SERVER_PORT 8080



 int clientSocket;
// unsigned char sendbuf[200];
 void sendmsge()
{
	unsigned char sendbuf[1024];
	while(1)
	{
	printf("send message to other client:\n");
 	scanf("%s",sendbuf);
        send(clientSocket,sendbuf,strlen(sendbuf), 0);
        if (strcmp(sendbuf, "quit") == 0)
           break;
        }
}

int main()
{

    pthread_t tid[100]={0};
    pthread_detach(pthread_self());
    int g=0;

    //客户端只需要一个套接字文件描述符，用于和服务器通信
   
    int oldtype;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&oldtype);
    //描述服务器的socket
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    
    
    int datanum;

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return 1;

    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    //指定服务器端的ip
    //inet_addr()函数，将点分十进制IP转换成网络字节序IP
//    serverAddr.sin_addr.s_addr = inet_addr("10.200.2.158");
    serverAddr.sin_addr.s_addr = inet_addr("192.168.17.128 ");
  
    if (connect(clientSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
    {

        perror("connect");

        return 1;

    }
    
       printf("连接到server...\n");
   
       

 //       pthread_create(&tid[g],NULL,(void *)send,(void *)&clientSocket);
    if((pthread_create(&tid[g],NULL,(void *)sendmsge,(void *)&clientSocket)) != 0)
    {
        perror("pthread_create");
        exit(1);
    }
        
 
 //    pthread_join(tid[g],NULL);  



while(1)
{
     unsigned char recvbuf[200];
     printf("receieve message from server:\n");
     recvbuf[0] = '\0';
     datanum=recv(clientSocket,recvbuf,200,0);
     recvbuf[datanum]='\0';
     printf("%s\n",recvbuf);
     
    
}  
  close(clientSocket);
    return 0;

}


  
