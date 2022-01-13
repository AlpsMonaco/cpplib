#include "network.h"

#ifdef _WIN32
#include "WinSock2.h"
#pragma comment(lib, "WS2_32.lib")
#else
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/unistd.h>

#endif

using namespace network;
int Socket::GetPort() { return this->port; }
const char *Socket::GetAddr() { return this->errmsg; }

Socket::Socket(int fd, const char *addr, const int &port)
{
	this->fd = fd;
	this->port = port;
	this->addr = (char *)malloc(strlen(addr) + 1);
	strcpy(this->addr, addr);
	this->errcode = 0;
	this->errmsg = nullptr;
}

void Socket::SetError(const int &errcode, const char *errmsg)
{
	this->errcode = errcode;
	if (this->errmsg != nullptr)
		free(this->errmsg);
	this->errmsg = (char *)malloc(strlen(errmsg) + 1);
	strcpy(this->errmsg, errmsg);
}

int Socket::Send(const char *buf, const int &bufSize)
{
	int size = send(fd, buf, bufSize, 0);
	if (size == -1)
		this->SetError(size, "send failed");
	return size;
}

int Socket::Recv(char *buf, const int &bufSize)
{
	int size = recv(fd, buf, bufSize, 0);
	this->SetError(size, "recv failed");
	return size;
}

inline void SetErrorMsg(int *errcode, char **errmsg, int code, char *msg)
{
	*errcode = code;
	if (*errmsg != nullptr)
		free(*errmsg);
	*errmsg = (char *)malloc(strlen(msg) + 1);
	strcpy(*errmsg, msg);
}

#ifdef _WIN32

void Clean()
{
	WSACleanup();
}

struct WSAManager
{
	bool isInit;
	WSADATA wsadata;
	void Init()
	{
		if (!isInit)
			WSAStartup(MAKEWORD(2, 2), &wsadata);
		isInit = true;
		atexit(Clean);
	}

} wsa{false};

Socket::Socket(const char *addr, const int &port, const int &af, const int &sock)
{
	this->port = port;
	this->af = af;
	this->sock = sock;
	this->addr = (char *)malloc(strlen(addr) + 1);
	strcpy(this->addr, addr);
	this->errcode = 0;
	this->errmsg = nullptr;
	wsa.Init();
	this->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->fd == INVALID_SOCKET)
	{
		SetErrorMsg(&this->errcode, &this->errmsg, INVALID_SOCKET, "Socket is invalid");
		return;
	}
}

Socket::~Socket()
{
	free(this->addr);
	if (this->errmsg != nullptr)
		free(this->errmsg);
}

int Socket::Close() { return closesocket(this->fd); }

#else

Socket::Socket(const char *addr, const int &port, const int &af, const int &sock)
{
	this->port = port;
	this->af = af;
	this->sock = sock;
	this->addr = (char *)malloc(strlen(addr) + 1);
	strcpy(this->addr, addr);
	this->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->fd == -1)
	{
		this->SetError(-1, "invalid socket");
		return;
	}
}

Socket::~Socket()
{
	free(this->addr);
	if (this->errmsg != nullptr)
		free(this->errmsg);
}

int Socket::Close() { return close(this->fd); }

#endif

using namespace tcp;
#ifdef _WIN32

Client::Client(const char *addr, const int &port) : Socket(addr, port, AF_INET, SOCK_STREAM)
{
}
Client::~Client() {}
bool Client::Connect()
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->port);
	addr.sin_addr.S_un.S_addr = inet_addr(this->addr);
	int ret = connect(fd, (sockaddr *)&addr, sizeof(addr));
	if (ret)
	{
		SetError(ret, "connect failed");
		return false;
	}
	return true;
}

Server::Server(const char *addr, const int &port) : Socket(addr, port, AF_INET, SOCK_STREAM) {}
Server::~Server() {}
bool Server::Listen()
{
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->port);
	addr.sin_addr.S_un.S_addr = inet_addr(this->addr);
	int ret = bind(fd, (sockaddr *)&addr, sizeof(addr));
	if (ret)
	{
		SetErrorMsg(&this->errcode, &this->errmsg, ret, "bind failed");
		return false;
	}
	ret = listen(fd, 10);
	if (ret)
	{
		SetErrorMsg(&this->errcode, &this->errmsg, ret, "listen failed");
		return false;
	}
	return true;
}

#else
Client::Client(const char *addr, const int &port) : Socket(addr, port, AF_INET, SOCK_STREAM)
{
}
Client::~Client() {}
bool Client::Connect()
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->port);
	addr.sin_addr.s_addr = inet_addr(this->addr);
	int ret = connect(fd, (sockaddr *)&addr, sizeof(addr));
	if (ret)
	{
		SetError(ret, "connect failed");
		return false;
	}
	return true;
}

#endif