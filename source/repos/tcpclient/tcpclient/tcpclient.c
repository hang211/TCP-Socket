// tcpclient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
//#include <netdb.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <unistd.h>
#include <windows.h>
#include <winsock.h>  


#define SERVER_PORT 8080

/*
���ӵ��������󣬻᲻ͣѭ�����ȴ����룬
����quit�󣬶Ͽ��������������
*/

int main() {

    //�ͻ���ֻ��Ҫһ���׽����ļ������������ںͷ�����ͨ��
    int clientSocket;

    //������������socket
    struct sockaddr_in serverAddr;
    char sendbuf[200];
    char recvbuf[200];
    int iDataNum;

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return 1;

    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    //ָ���������˵�ip�����ز��ԣ�10.200.2.158
    //inet_addr()�����������ʮ����IPת���������ֽ���IP
    serverAddr.sin_addr.s_addr = inet_addr("10.200.2.158");
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {

        perror("connect");

        return 1;

    }

    printf("���ӵ�����...\n");
    while(TRUE)
    {
        printf("������Ϣ:");
        scanf("%s", sendbuf);
        printf("\n");
        send(clientSocket, sendbuf, strlen(sendbuf), 0);
        if (strcmp(sendbuf, "quit") == 0)
            break;

        printf("��ȡ��Ϣ:");
        recvbuf[0] = '\0';
        iDataNum = recv(clientSocket, recvbuf, 200, 0);
        recvbuf[iDataNum] = '\0';
        printf("%s\n", recvbuf);

    }

    close(clientSocket);
    return 0;

}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
