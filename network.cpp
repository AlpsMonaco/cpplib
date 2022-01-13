#include "network.h"

#ifdef _WIN32
#include "WinSock2.h"
#pragma comment(lib, "WS2_32.lib")
#else
#define INVALID_SOCKET -1
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/unistd.h>

#endif

using namespace network;
int Socket::GetPort() { return this->port; }
const char *Socket::GetAddr() { return this->addr; }
inline void SetErrorMsg(int *errcode, char **errmsg, int code, char *msg)
{
	*errcode = code;
	if (*errmsg != nullptr)
		free(*errmsg);
	*errmsg = (char *)malloc(strlen(msg) + 1);
	strcpy(*errmsg, msg);
}

Socket::Socket(const Socket &s)
{
	this->errcode = s.errcode;
	this->port = s.port;
	this->fd = s.fd;
	this->af = s.af;
	this->sock = s.sock;
	this->addr = nullptr;
	this->errmsg = nullptr;
	if (s.errmsg != nullptr)
	{
		this->errmsg = (char *)malloc(strlen(s.errmsg) + 1);
		strcpy(this->errmsg, s.errmsg);
	}
	if (s.addr != nullptr)
	{
		this->addr = (char *)malloc(strlen(s.addr) + 1);
		strcpy(this->addr, s.addr);
	}
}

Socket::Socket(Socket &&s)
{
	this->af = s.af;
	this->sock = s.sock;
	this->addr = s.addr;
	this->port = s.port;
	this->fd = s.fd;
	this->errmsg = s.errmsg;
	this->errcode = s.errcode;
	s.af = 0;
	s.sock = 0;
	s.addr = nullptr;
	s.port = 0;
	s.fd = 0;
	s.errmsg = nullptr;
	s.errcode = 0;
}

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

Socket::Socket(const char *addr, const int &port, const int &af, const int &sock)
{
	this->port = port;
	this->af = af;
	this->sock = sock;
	this->addr = (char *)malloc(strlen(addr) + 1);
	strcpy(this->addr, addr);
	this->errcode = 0;
	this->errmsg = nullptr;
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
	{
		free(this->errmsg);
		this->errmsg = nullptr;
	}
}

#ifdef _WIN32

void Clean()
{
	WSACleanup();
}

struct WSAManager
{
	WSAManager()
	{
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);
		atexit(Clean);
	}
} wsa;

int Socket::Close() { return closesocket(this->fd); }

#else

int Socket::Close()
{
	return close(this->fd);
}

#endif

using namespace tcp;
Client::Client(const char *addr, const int &port) : Socket(addr, port, AF_INET, SOCK_STREAM) {}
Client::~Client() {}
Client::Client(Client &&c) : Socket((Client &&) c) {}

#ifdef _WIN32

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