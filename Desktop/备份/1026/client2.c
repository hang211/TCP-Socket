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


#define SERVER_PORT 8080
/*
连接到服务器后，会不停循环，等待输入，
输入quit后，断开与服务器的连接
*/


//High-Order Byte Table
/* Table of CRC values for highorder byte */
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




int main()
{
    pthread_t tid[100]={0};
    pthread_detach(pthread_self());
    int g=0;
    //客户端只需要一个套接字文件描述符，用于和服务器通信
    int clientSocket;
    int oldtype;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,&oldtype);
    //描述服务器的socket
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    unsigned char sendbuf[200];
    char recvbuf[200];
    int iDataNum;

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
    
    

    unsigned char data[100];
    unsigned char b[16];
    int c[1];

    struct timedata
    {
        int yn;
        int mn;
        int dn;
        int hn;
        int minn;
        int secn;
        int milisec;
    } timedata;


    time_t nSeconds;
    struct tm * pTM;
    //struct  timeb   stTimeb;
    struct  timespec   stTimeb;
    //ftime(&stTimeb);
    clock_gettime(CLOCK_MONOTONIC,&stTimeb);

    time(&nSeconds);
    pTM = localtime(&nSeconds);

    timedata.yn=pTM->tm_year + 1900;
    timedata.mn=pTM->tm_mon + 1;
    timedata.dn=pTM->tm_mday;
    timedata.hn=pTM->tm_hour;
    timedata.minn=pTM->tm_min;
    timedata.secn=pTM->tm_sec;
//	timedata.milisec=nSeconds.millitm;





    while (1)
    {
    
    pthread_create(&tid[g],NULL,(void *)send,(void *)&clientSocket);
    if((pthread_create(&tid[g],NULL,(void *)send,(void *)&clientSocket)) != 0)
    {
        perror("pthread_create");
        exit(1);
    }
    
    
    pthread_create(&tid[g],NULL,(void *)recv,(void *)&clientSocket);
    if((pthread_create(&tid[g],NULL,(void *)recv,(void *)&clientSocket)) != 0)
    {
        perror("pthread_create2");
        exit(1);
    }

    pthread_join(tid[g],NULL);
 
    
    
        int pos=0;
        int d=0;
        unsigned char a[6];
        a[0]=0x55;
        d+=1;
        a[d++]=0x27;
        a[d++]=0xE3;
        a[d++]=0;
        a[d++]=1;
        a[d++]=0xF1;
        printf("dev_id:");
        scanf("%s",b);
        for(int j=0; j<16; j++)
        {
            data[j]=b[j];
        }
        pos+=16;
        printf("command:");
        scanf("%d",c);
        data[pos++]=2;
        data[pos++] = (31 >> 8)&0xFF ;
        data[pos++] = 31 & 0xFF ;

        data[pos++]=c[0];
        data[pos++]=(timedata.yn>>8)&0xFF;
        data[pos++]=(timedata.yn)&0xFF;
        data[pos++]=timedata.mn;
        data[pos++]=timedata.dn;
        data[pos++]=timedata.hn;
        data[pos++]=timedata.minn;
        data[pos++]=timedata.secn;
 //       data[pos++]=(stTimeb.millitm>>8)&0xFF;
 //       data[pos++]=(stTimeb.millitm)&0xFF;

//        printf("%d\n",stTimeb.millitm);

//        data[pos++] =(int)0x01;


//	gettimeofday(&time,null);
//	data[pos++]=time

//	printf("%02X",data[0]);
        for(int n=0; n<d; n++)
        {
            printf("%02X",a[n]);
            sendbuf[n]=a[n];
        }
        for(int k=0; k<pos; k++)
        {
            printf("%02X",data[k]);
            sendbuf[k+6]=data[k];
        }
        unsigned short crc16Data = CRC16(data, pos);
        printf("%04X", crc16Data);
        unsigned char s;
        for(int u =1; u<6; u++)
        {
            s+=a[u];
        }
        for(int m=0; m<pos; m++)
        {
            s+=data[m];
        }
        unsigned char checkcodeh=crc16Data>>8;
        s=s+crc16Data+checkcodeh;
        sendbuf[35]=checkcodeh;
        sendbuf[36]=crc16Data;
        sendbuf[37]=s;
        sendbuf[38]=0xAA;
        printf("%02X",s);
        printf("AA\n");

//	data[pos++]=s;
//	data[pos++]=0xAA;




//	bin2hex($data)
        printf("send:");

        send(clientSocket,sendbuf,39, 0);

        if (strcmp(sendbuf, "quit") == 0)
     //   pthread_cancel(tid[1]);
            break;

        printf("receieve message from server:");
        recvbuf[0] = '\0';
        iDataNum = recv(clientSocket, recvbuf, 200, 0);
        recvbuf[iDataNum] = '\0';
        printf("%s\n", recvbuf);
         printf("send message to other client:");

        send(clientSocket,sendbuf,strlen(sendbuf), 0);

    }

    close(clientSocket);
    return 0;

}
