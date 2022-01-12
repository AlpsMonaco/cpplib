#include "network.h"

#ifdef _WIN32
#include "WinSock2.h"
#pragma comment(lib, "WS2_32.lib")
#else

#endif

using namespace network;
Socket::Socket(const char *addr, const int &port, const int &af, const int &sock)
{
	this->port = port;
	this->af = af;
	this->sock = sock;
	this->addr = (char *)malloc(strlen(addr) + 1);
	strcpy(this->addr, addr);
	this->errmsg = nullptr;
	this->fd = 0;
}

inline int socketsend(int fd, const char *buf, const int &bufSize, int *errcode, char **errmsg)
{
	int size = send(fd, buf, bufSize, 0);
	if (size == -1)
	{
		*errcode = -1;
		if (*errmsg != nullptr)
		{
			const char *msg = "Send Failed";
			free(*errmsg);
			if (*errmsg != nullptr)
			{
				free(*errmsg);
			}
			*errmsg = (char *)malloc(strlen(msg) + 1);
			strcpy(*errmsg, msg);
		}
	}
	return size;
}

inline int socketrecv(int fd, char *buf, const int &bufSize, int *errcode, char **errmsg)
{
	int size = recv(fd, buf, bufSize, 0);
	if (size == -1)
	{
		*errcode = -1;
		if (*errmsg != nullptr)
		{
			const char *msg = "Recv Failed";
			free(*errmsg);
			if (*errmsg != nullptr)
			{
				free(*errmsg);
			}
			*errmsg = (char *)malloc(strlen(msg) + 1);
			strcpy(*errmsg, msg);
		}
	}
	return size;
}

Socket::~Socket()
{
	free(this->addr);
	if (this->errmsg != nullptr)
		free(this->errmsg);
}

void Socket::SetError(const int &errcode, const char *errmsg)
{
	this->errcode = errcode;
	if (this->errmsg != nullptr)
		free(this->errmsg);
	this->errmsg = (char *)malloc(strlen(errmsg) + 1);
	strcpy(this->errmsg, errmsg);
}

int Socket::Errno() { return this->errcode; }
const char *Socket::Error() { return this->errmsg; }

using namespace tcp;
#ifdef _WIN32

Client::Client(const char *addr, const int &port) : Socket(addr, port, AF_INET, SOCK_STREAM)
{
}

bool Client::Connect()
{
	WSADATA wsadata;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (ret)
	{
		SetError(ret, "WSAStartup init failed.");
		return false;
	}
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == INVALID_SOCKET)
	{
		SetError(INVALID_SOCKET, "Socket is invalid");
		return false;
	}
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->port);
	addr.sin_addr.S_un.S_addr = inet_addr(this->addr);
	ret = connect(fd, (sockaddr *)&addr, sizeof(addr));
	if (ret)
	{
		SetError(ret, "connect failed");
		return false;
	}
	return true;
}

int Client::Send(const char *buf, const int &bufSize)
{
	return socketsend(fd, buf, bufSize, &this->errcode, &this->errmsg);
}

int Client::Recv(char *buf, const int &bufSize)
{
	return socketrecv(fd, buf, bufSize, &this->errcode, &this->errmsg);
	// int size = recv(fd, buf, bufSize, 0);
	// if (size == -1)
	// 	SetError(-1, "Recv failed");
	// return size;
}

int Client::Close()
{
	closesocket(fd);
	WSACleanup();
	return 0;
}

#else

#endif