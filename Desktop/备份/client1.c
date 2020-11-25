#include<sys/socket.h>
#include<string.h>
#include<linux/in.h>
#include<sys/types.h>
#include<stdio.h>
#include<stdlib.h>
/*流程：

TCP：(面向连接、可靠)

服务器端 WSAStartup->socket->bind->listen->accept->recv/send->closesocket->WSACleanup
客户端：WSAStartup->socket->connect->recv/send->closesocket->WSACleanup
*/
#define PORT 8080
int main()
{
    int csocfd;
    int recdata;    
    char buf[1024];
    memset(buf,0x00,sizeof(buf));
    struct sockaddr_in mysockaddr;
    //创建套接字
    if(-1 == (csocfd = socket(PF_INET,SOCK_STREAM,0))){
            printf("csocfd failed!\n");
            return -1;
    }
    printf("csocfd suc!\n");
    
    //设置服务器的地址

    memset(&mysockaddr,0x00,sizeof(mysockaddr));
    mysockaddr.sin_family = PF_INET;
    mysockaddr.sin_port = htons(PORT);
    inet_pton(AF_INET,"10.200.2.158",&mysockaddr.sin_addr.s_addr);
//s_add.sin_addr.s_addr= inet_addr("10.200.2.158"); /* ip转换为4字节整形，使用时需要根据服务端ip进行更改 */  


    //connect to the sr
if(-1 == connect(csocfd,(struct sockaddr*)&mysockaddr,sizeof(mysockaddr))){
            printf("connect failed!\n");
            return -1;
        }
    printf("connect suc!\n");
    
    
    if(-1 == (recdata = read(csocfd,buf,sizeof(buf)))){
            printf("read failed!\n");
            return -1;
    }
    printf("read suc!\n");
    buf[recdata ] = '\0';
    printf("recdata  = [%s]\n",buf);
    printf("recdata len is = [%d]\n",recdata);
    
    
    //memcpy(sendbuf,"this is data from cl!\n",);
    if(-1 == send(csocfd, "this is data from cl!", 100, 0)){
            printf("send faile\n");
            return -1;
        }
    printf("send suc!\n");
    
    
    close(csocfd);
    
    
}
