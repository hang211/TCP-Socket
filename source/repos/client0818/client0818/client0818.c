// client0818.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")  
#include <WinSock2.h>



WORD socketverson = MAKEWORD(2, 2);
WSADATA wsadata;
if (WSAStartup(socketverson, &wsadata) != 0)
return0;

SOCKET clientsocket =socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
if (clientsocket==INVALID_SOCKET)
{
	printf("�׽��ִ���ʧ��")��
	WSACleanup();
	return 0;
}

sockaddr_in addrConserver;
addrConserver.sin_familly = AF_INET;
addrConserver.sin_port = htons(8080);
addrConserver.sin_addr.S_un.S_addr = inet_addr("10.200.2.158");

if (connect(clientsocket, (SOCKADDR*)&addrConserver, sizeof(addrConserver)) == SOCKET_ERROR)
{
	printf("�ͻ�������ʧ�ܣ�\n");
	closesocket(clientsocket);
	WSACleanup();
	return 0;
}
else
printf("�ͻ��˽������ӳɹ���׼���������ݣ�\n");

int sendRes = send(clientsocket, sendBuf, (int)strlen(sendBuf), 0);
if (sendRes == SOCKET_ERROR)
{
	printf("�ͻ���send()���ִ��� : %d\n", WSAGetLastError());
	closesocket(clientSocket);
	WSACleanup();
	return 0;
}
else
printf("�ͻ��˷������ݳɹ�!\n");

//���շ��������
/*ͨ�����������ӽ���ͨ��*/
do
{
	char revSerData[100] = "";
	res = recv(clientSocket, revSerData, sizeof(revSerData), 0);
	if (res > 0)
	{
		printf("Bytes received: %d\n", res);
		printf("���������͵�����: %s\n", revSerData);
	}
	else if (res == 0)
		printf("Connection closed\n");
	else
		printf("recv failed: %d\n", WSAGetLastError());
}
while (res > 0);

closesocket(clientsocket);
WSACleanup();
int main()
{

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
