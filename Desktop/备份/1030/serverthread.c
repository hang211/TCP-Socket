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
#include <pthread.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <ctype.h>
#define SERVER_PORT 8080

//crc16 checkcode
static unsigned char auchCRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
};
//Low-Order Byte Table
/* Table of CRC values for loworder byte */
static unsigned char auchCRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
} ;

//CRC Generation Function
unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen)
// unsigned char *puchMsg ; /* message to calculate CRC upon */
// unsigned short usDataLen ; /* quantity of bytes in message */
{
    unsigned char uchCRCHi = 0xFF ; /* high byte of CRC initialized */
    unsigned char uchCRCLo = 0xFF ; /* low byte of CRC initialized */
    unsigned uIndex; /* will index into CRC lookup table */
    while (usDataLen--) /* pass through message buffer */
    {
        uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
        uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ;
        uchCRCLo = auchCRCLo[uIndex] ;
    }
    return (uchCRCHi << 8 | uchCRCLo) ;
}




	typedef void *(THREAD_BODY) (void *thread_arg);	
	void *thread_worker(void *ctx);
	//封装thread_start()函数，实现创建子线程的功能
	int thread_start(pthread_t * thread_id, THREAD_BODY * thread_workbody, void *thread_arg);


int g=0;


int main(int argc, char **argv)
{
//	int serverSocket;
	long client[100]={0};
	int h=0;
	
//	int ch;
	int on = 1;
//	int port = 0;
	pthread_t tid[100]={0};
	pthread_detach(pthread_self());

    //调用socket函数返回的文件描述符
    int serverSocket;

    //声明两个套接字sockaddr_in结构体变量，分别表示客户端和服务器

    struct sockaddr_in server_addr;

    struct sockaddr_in clientAddr;

    int addr_len = sizeof(clientAddr);

  //  int client;

    unsigned char buffer[200];

    int datalength;
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        return 1;
    }

    bzero(&server_addr, sizeof(server_addr));


    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);


    server_addr.sin_addr.s_addr = inet_addr("192.168.17.128 ");
     int reuse=1;
     if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
       {
                perror("setsockopet error\n");
                return -1;
       }




    if (bind(serverSocket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        return 1;
    }
    

    while (1)
    {
	

    


    if (listen(serverSocket, 13) < 0)
    {
        perror("listen");
        return 1;

    }

        printf("Listening to client...\n");


        client[h] = accept(serverSocket, (struct sockaddr *) &clientAddr, (socklen_t *) &addr_len);

        if (client < 0)
        {
            perror("accept");
            continue;
        }
//	pthread_create(&tid[g],NULL,(void *)recv,(void *)&client[h]);
 	thread_start(&tid[g], thread_worker, (void *)client[h]);
 	g++;
 	
	printf("start to commuicate with client...\n");
	printf("number of online clients currently is %d\n",g);

     
//start communication
        //while (1)
     //   {

            buffer[0] = '\0';
            datalength = recv(client[h], buffer, 1024, 0);

            if (datalength < 0)
            {
                perror("recv null");
                continue;
            }

            buffer[datalength] = '\0';
            if (strcmp(buffer, "quit") == 0)
                break;
//	printf("%02X\n",datalength);
            printf("receive:");
            for(int k=0; k<datalength; k++)
            {
                printf("%02X",buffer[k]);
            }
            printf("\n");

//
            if((buffer[0]==0x55)&&(buffer[datalength-1]==0xAA))
            {
                if(buffer[1]==datalength)
                {
                    unsigned char sum=0;
                    for(int i=1; i<datalength-2; i++)
                    {
                        sum+=buffer[i];
                    }
                    printf("checksum=%02X,correct.\n",sum);


                    printf("device_type:%02X %02X %02X\n",buffer[2],buffer[3],buffer[4]);
                    printf("cmd:%02X\n",buffer[5]);
                    printf("cmd_arg_len:%02d\n",datalength-8);

                    unsigned char data[datalength-8];
                    char b[17]="";
                    for(int j=0; j<datalength-8; j++)
                    {
                        data[j]=buffer[j+6];
                    }
                    printf("\n");
                    printf("Variable length data:");

                    if(buffer[2]==0xE3)
                    {

                        if(datalength-8>0)
                        {
                            if((datalength-8-21)>0)
                            {
                                for(int k=0; k<datalength-8; k++)
                                {
                                    printf("%02X ",data[k]);
                                }
                                printf("\n");

                                if(((data[17]<<8&0xFF00)+(data[18]&0xFF))==datalength-8 )
                                {

                                    unsigned short crc16Data = CRC16(data, datalength-10);
                                    if(crc16Data==(((buffer[datalength-4]<<8)&0xFF00)+(buffer[datalength-3]&0xFF)))
                                    {
                                        int year;
                                        year=(((data[20]<<8)&0xFF00)+(data[21]&0xFF));
                                        int millisec;
                                        millisec=((data[27]<<8)&0xFF00)+(data[28]&0xFF);

                                        printf("product id:");
                                        for(int m=0; m<16; m++)
                                        {
                                            b[m]=data[m];
                                        }
                                        printf("%s",b);

                                        printf("\n");

                                        printf("data type:%d\n",data[16]);

                                        if(data[16]==1)
                                        {
                                            printf("[Upload telemetry data]...\n");
                                            switch(data[19])
                                            {
                                            case 1:
                                            {
                                                printf("command type:");
                                                printf("%d(General basic parameter blow)\n",data[19]);
                                                printf("command timestamp_a: ");
                                                printf("%04d-%02d-%02d %02d:%02d:%02d.%03d\n",year,data[22],data[23],data[24],data[25],data[26],millisec);

                                            }
                                            break;
                                            case 255:
                                            {
                                                printf("command type:");
                                                printf("%d(Heartbeat packet blow)\n",data[19]);
                                                printf("command timestamp_a: ");
                                                printf("%04d-%02d-%02d %02d:%02d:%02d.%03d\n",year,data[22],data[23],data[24],data[25],data[26],millisec);

                                            }
                                            default:
                                                printf("invalid data\n");
                                                break;

                                            }
                                        }
                                        if(data[16]==2)
                                        {
                                            printf("[The upper computer gives orders]...\n");
                                            switch(data[19])
                                            {

                                            case 1:
                                            {
                                                printf("command type:");
                                                printf("%d(Set the real time lights on)\n",data[19]);
                                                printf("command timestamp_a: ");
                                                printf("%04d-%02d-%02d %02d:%02d:%02d.%03d\n",year,data[22],data[23],data[24],data[25],data[26],millisec);
                                            }
                                            break;

                                            case 3:
                                            {

                                                printf("command type:");
                                                printf("%d(Set the lights to turn off in real time)\n",data[19]);
                                                printf("command timestamp_a: ");
                                                printf("%04d-%02d-%02d %02d:%02d:%02d.%03d\n",year,data[22],data[23],data[24],data[25],data[26],millisec);

                                            }
                                            break;

                                            case 5:
                                            {
                                                printf("%d(Synchronous clock timing)\n",data[19]);
                                            }
                                            break;

                                            default:
                                                printf("invalid data\n");
                                                break;
                                            }


                                        }
                                        else
                                            printf("checkcode is incorrect\n");
                                    }
                                    else
                                        printf("data length is incorrect\n");


                                }

                                else
                                    printf("Variable length data is invalid.\n");


                            }
                            else
                            {
                                printf("empty!\n");
                                printf("\n");
                            }
                        }
                    }
                    else
                    {
                        printf("data length is incorrect\n");
                        printf("\n");
                    }
                }
                else
                {
                    printf("invalid data!\n");
                    printf("\n");
                }

                printf("\n");
                printf("send message to clients:");
                scanf("%s", buffer);
                printf("\n");
                send(client[h], buffer, strlen(buffer), 0);

                if (strcmp(buffer, "quit") == 0)
                    break;
            }
         

        }

        close(serverSocket);
        return 0;    
//}

}

int thread_start(pthread_t *thread_id, THREAD_BODY *thread_workbody, void *thread_arg)
{
	int 				recv = -1;
	pthread_attr_t 		thread_attr;
	
	if (pthread_attr_init(&thread_attr))
	{
		printf("pthread_attr_init() failure: %s\n", strerror(errno));
		goto CleanUp;
	}
	
	if (pthread_attr_setstacksize(&thread_attr, 120*1024))
	{
		printf("pthread_attr_setstacksize() failure: %s\n", strerror(errno));
					goto CleanUp;
	}
	
	if (pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED))
	{
		printf("pthread_attr_setdetachstate() failure: %s\n", strerror(errno));
		goto CleanUp;
	}
	
	/* Create the thread */
	if (pthread_create(thread_id, &thread_attr, thread_workbody, thread_arg))
	{
		printf("Create thread failure: %s\n", strerror(errno));
		goto CleanUp;
	}
	
	recv = 0;
CleanUp:
	/* Destroy the attributes of thread */
	pthread_attr_destroy(&thread_attr);
	return recv;
}

void *thread_worker(void *ctx)
{
	    
    
	int client[100]={0};
	int h=0;
	
	int recv;
	unsigned char buf[1024];


	int i;
	client[h]=(long)ctx;
	if (!ctx)
	{
		printf("Invalid input arguments in %s()\n", __FUNCTION__);
		pthread_exit(NULL);
	}
	
//    send(clientSocket,buf,strlen(buf));
	
	
	
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		recv = read(client[h], buf, sizeof(buf));
		if (recv < 0)
		{
			printf("Read data from client sockfd[%d] failure: %s and thread will exit\n", client[h],
				 	  strerror(errno));
			
				 	  
			close(client[h]);
			pthread_exit(NULL);
		}
		else if( recv == 0)
		{
			printf("client[%d] get disconnected and thread will exit.\n", g);
			close(client[h]);
			g--;
			printf("number of online client is %d\n",g);
			pthread_exit(NULL);
			
		}
		else if( recv > 0 )
		{
			printf("Read %d bytes data from client[%d]: \n", recv,h);
			for(int z=0;z<=h;z++)
			{
			  send(client[z],(void *)recv,sizeof(recv),0);
			  printf("send sucess\n");
			 } 
		//	send(client[h],buf,strlen(buf));

            		for(int k=0; k<39; k++)
          	 	 {
             		 	  printf("%02X",buf[k]);
          	 	 }
           		 printf("\n");

			
		}
		
		/* convert letter from lowercase to uppercase */
		for (i = 0; i < recv; i++)
		{
			buf[i] = toupper(buf[i]);
		}
		recv = write(client[h], buf, recv);
		if (recv < 0)
		{
			printf("Write to client by sockfd[%d] failure: %s and thread will exit\n", client[h],
					strerror(errno));
			close(client[h]);
			pthread_exit(NULL);
		}
	}
}


