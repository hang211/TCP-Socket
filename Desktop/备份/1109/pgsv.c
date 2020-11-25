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

typedef void *(THREAD_BODY) (void *thread_arg);	
	void *thread_worker(void *ctx);
//封装thread_start()函数，实现创建子线程的功能
int thread_start(pthread_t * thread_id, THREAD_BODY * thread_workbody, void *thread_arg);
int client[100]={0};
int h=0;
int g=0;
int t=0;
//unsigned char buf[1024];

void *sendmsge()
{
unsigned char sendbuf[200];
while(1)
{
	printf("\n");
	printf("send message to clients:");
	scanf("%s", sendbuf);	
	printf("\n");
	
        for(int x=0;x<h;x++)
       {
         send(client[x], sendbuf, strlen(sendbuf), 0);
         printf("send out susess\n");
       }

        if (strcmp(sendbuf, "quit") == 0)
              break;
	
}


}


int main(int argc, char **argv)
{
	
//	int client=0;
	int on = 1;
	pthread_t tid[100]={0};
	pthread_detach(pthread_self());
	int serverSocket;
	struct sockaddr_in server_addr;
	struct sockaddr_in clientAddr;
	int addr_len = sizeof(clientAddr);
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
        printf("start to commuicate with client...\n");
	 printf("number of online clients currently is %d\n",g);

//	 h++;
	
        long listen_client = accept(serverSocket, (struct sockaddr *) &clientAddr, (socklen_t *) &addr_len);
       printf("listen=%ld\n",listen_client);

        	if (listen_client< 0)
        	{
           	 perror("accept");
          	  continue;
        	}
 	thread_start(&tid[g], thread_worker, (void *)listen_client);
	
 	g++;
 	thread_start(&tid[t], sendmsge, (void *)listen_client);
	t++;
}
	
	close(serverSocket);
        return 0;  

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
	    
    
//	int client[100]={0};
//	int h=0;
	//h++;
	int recv;
	
	


	int i;
	if (!ctx)
	{
		printf("Invalid input arguments in %s()\n", __FUNCTION__);
		pthread_exit(NULL);
	}
	client[h]=(long)ctx;
	long client_me = client[h];
	printf("client[%d]=%d\n",h, client[h]);
	h++;
	
	
	
	while (1)
	{
		
	/*        printf("\n");
                printf("send message to clients:");
                printf("h=%d\n",h);
                scanf("%s", buf);
                printf("\n");
                for(int x=0;x<h;x++)
                {
                send(client[x], buf, strlen(buf), 0);
                }

                if (strcmp(buf, "quit") == 0)
                    break;
	*/
	
		unsigned char buf[1024];
		memset(buf, 0, sizeof(buf));
		recv = read(client_me, buf, sizeof(buf));
		if (recv < 0)
		{
			printf("Read data from client sockfd[%d] failure: %s and thread will exit\n", h,
				 	  strerror(errno));
			
				 	  
			close(client_me);
			pthread_exit(NULL);
		}
		else if( recv == 0)
		{
			printf("client[%d] get disconnected and thread will exit.\n", g);
			close(client_me);
			g--;
			printf("number of online client is %d\n",g);
			pthread_exit(NULL);
			
		}
		else if( recv > 0 )
		{

			printf("Read %d bytes data from client[%d]: \n",recv,h);
			printf("%s\n",buf);
		//	send(client_me,buf,strlen(buf),0);
			for(int z=0;z<h;z++)
			{
			  if(client[z]!=client_me)
			  {
			  send(client[z],buf,strlen(buf),0);
			  }
			}
			
		}
		
		/* convert letter from lowercase to uppercase */
		/*
		for (i = 0; i < recv; i++)
		{
			buf[i] = toupper(buf[i]);
		}
		recv = write(client_me, buf, recv);
		if (recv < 0)
		{
			printf("Write to client by sockfd[%d] failure: %s and thread will exit\n", client_me,
					strerror(errno));
			close(client_me);
			pthread_exit(NULL);
		}*/
	}
}
