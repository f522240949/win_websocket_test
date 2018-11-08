#include <stdio.h>
#include <stdlib.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <string.h>
#pragma comment(lib,"ws2_32.lib")

void cat(int client, FILE* resource);

int main(int argc, char* argv[])
{
	//初始化socket
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;

	int ret = WSAStartup(sockVersion, &wsaData);
	if (0 != ret)
	{
		printf("WSAStartup error");
		return -1;
	}
	//创建监听
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET)
	{
		printf("slisten error !");
		return -1;
	}

	//服务器绑定需要的IP和端口
	sockaddr_in sin;//addr
	sin.sin_family = AF_INET;				//ipv4
	sin.sin_port = htons(80);				//web 端口都是 80 端口
	sin.sin_addr.S_un.S_addr = INADDR_ANY;	//任意地址都可以访问

	ret = bind(slisten, (LPSOCKADDR)&sin, sizeof(sin));
	if (ret == SOCKET_ERROR)
	{
		printf("bind error !");
		return -1;
	}

	//开始监听 等待接听
	ret = listen(slisten, 5);
	if (ret == SOCKET_ERROR)
	{
		printf("listen error !");
		return -1;
	}

	//连接成功后 接听 TCP
	SOCKET sClient;						//接收套接字
	sockaddr_in remoteAddr;				//对方地址
	int nAddrlen = sizeof(remoteAddr);	
	char revData[1024];					//储存接收的数据

	while (1)
	{
		Sleep(10);
		printf("等待连接...\n");
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		//连接的链子
		if (sClient == INVALID_SOCKET)
		{
			printf("sClient error !");
			continue;
		}
		char sendBuf[20] = { '\n' };//windows里面的换行 \r\n
		printf("收到一个连接：%s\r\n",inet_ntop(AF_INET,(void*)&remoteAddr.sin_addr, sendBuf, 16));

		ret = recv(sClient, revData, 1024, 0);
		if (ret > 0)
		{
			revData[ret] = 0x00;
			printf(revData);
		}

		char sendData[1024] = "HTTP/1.1 200 OK \r\n ";
		strcat(sendData, "Content-Type:text/html\r\n");
		strcat(sendData, "\r\n");

		//strcat(sendData, "<h1>hello c++ welcome<\h");
		send(sClient, sendData, strlen(sendData),0);

		const char*filename = "E:\\database.txt";
		FILE* resource = NULL;
		resource = fopen(filename, "r");
		cat(sClient, resource);

		closesocket(sClient);
	}

	system("pause");
}

//读文件 读Html 然后send给客户端
void cat(int client, FILE* resource)
{
	char buf[1024];
	fgets(buf, sizeof(buf), resource);//读取文件
	while (!feof(resource))
	{
		send(client, buf, strlen(buf), 0);
		fgets(buf, sizeof(buf), resource);
	}
}