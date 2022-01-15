#include "network.h"

#ifdef _WIN32
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <stringapiset.h>
using socklen_t = int;
#pragma comment(lib, "WS2_32.lib")
#else
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/unistd.h>
#include <errno.h>
#include <error.h>

#endif

namespace network
{
	void NetToHost(const void *net, char dst[16])
	{
		unsigned char *temp = (unsigned char *)net;
		for (int i = 0; i < 4; i++)
		{
			sprintf(dst, "%u", temp[i]);
			do
			{
			} while (*(++dst) != 0);

			if (i != 3)
			{
				*dst = '.';
				dst++;
			}
		}
	}

	// inline void SetErrorMsg(int *errcode, char **errmsg, const int &code, const char *msg)
	// {
	// 	*errcode = code;
	// 	if (*errmsg != nullptr)
	// 		free(*errmsg);
	// 	*errmsg = (char *)malloc(strlen(msg) + 1);
	// 	strcpy(*errmsg, msg);
	// }

	inline bool CreateSocket(int &fd, const int &af, const int &sock)
	{
		fd = socket(af, sock, 0);
		if (fd == -1)
			return false;
		return true;
	}

#ifdef _WIN32
	void Socket::SocketError()
	{
		wchar_t buf[256];
		this->errcode = WSAGetLastError();
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, this->errcode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, (sizeof(buf) / sizeof(wchar_t)), NULL);
		if (this->errmsg != nullptr)
			free(this->errmsg);
		int length = wcslen(buf) * 3 + 1;
		this->errmsg = (char *)malloc(length);
		wcstombs(this->errmsg, buf, length);
	}
#else
	void Socket::SocketError()
	{
		const int maxErrMsgSize = 128;
		this->errcode = errno;
		char buf[maxErrMsgSize];
		char *errmsg = strerror_r(this->errcode, buf, maxErrMsgSize);
		int length = strlen(errmsg);
		if (this->errmsg != nullptr)
			free(this->errmsg);
		this->errmsg = (char *)malloc(length + 1);
		strcpy(this->errmsg, errmsg);
	}
#endif

}

using namespace network;

int Socket::GetPort() { return this->port; }
const char *Socket::GetAddr() { return this->addr; }

Socket::Socket()
{
	this->errcode = 0;
	this->af = 0;
	this->sock = 0;
	this->port = 0;
	this->fd = 0;
	this->errmsg = nullptr;
	this->addr[0] = 0;
}

Socket &Socket::operator=(const Socket &s)
{
	this->errcode = s.errcode;
	this->port = s.port;
	this->fd = s.fd;
	this->af = s.af;
	this->sock = s.sock;
	this->addr[0] = 0;
	this->errmsg = nullptr;
	if (s.errmsg != nullptr)
	{
		this->errmsg = (char *)malloc(strlen(s.errmsg) + 1);
		strcpy(this->errmsg, s.errmsg);
	}
	strcpy(this->addr, s.addr);
	return *this;
}

Socket &Socket::operator=(Socket &&s)
{
	this->af = s.af;
	this->sock = s.sock;
	this->port = s.port;
	this->fd = s.fd;
	this->errmsg = s.errmsg;
	this->errcode = s.errcode;
	strcpy(this->addr, s.addr);
	s.af = 0;
	s.sock = 0;
	s.addr[0] = 0;
	s.port = 0;
	s.fd = 0;
	s.errmsg = nullptr;
	s.errcode = 0;
	return *this;
}

Socket::Socket(const Socket &s)
{
	this->errcode = s.errcode;
	this->port = s.port;
	this->fd = s.fd;
	this->af = s.af;
	this->sock = s.sock;
	this->addr[0] = 0;
	this->errmsg = nullptr;
	if (s.errmsg != nullptr)
	{
		this->errmsg = (char *)malloc(strlen(s.errmsg) + 1);
		strcpy(this->errmsg, s.errmsg);
	}
	strcpy(this->addr, s.addr);
}

Socket::Socket(Socket &&s)
{
	this->af = s.af;
	this->sock = s.sock;
	strcpy(this->addr, s.addr);
	this->port = s.port;
	this->fd = s.fd;
	this->errmsg = s.errmsg;
	this->errcode = s.errcode;
	s.af = 0;
	s.sock = 0;
	s.addr[0] = 0;
	s.port = 0;
	s.fd = 0;
	s.errmsg = nullptr;
	s.errcode = 0;
}

Socket::Socket(const int &af, const int &sock)
{
	this->af = af;
	this->sock = sock;
	this->errcode = 0;
	this->port = 0;
	this->fd = 0;
	this->errmsg = nullptr;
	this->addr[0] = 0;
}

Socket::Socket(int fd, const char *addr, const int &port, const int &af, const int &sock)
{
	this->fd = fd;
	this->addr[0] = 0;
	strcpy(this->addr, addr);
	this->port = port;
	this->af = af;
	this->sock = sock;
	this->errcode = 0;
	this->errmsg = nullptr;
}

int Socket::Send(const char *buf, const int &bufSize)
{
	int size = send(fd, buf, bufSize, 0);
	if (size == -1)
		this->SocketError();
	return size;
}

int Socket::Recv(char *buf, const int &bufSize)
{
	int size = recv(fd, buf, bufSize, 0);
	this->SocketError();
	return size;
}

Socket::Socket(const char *addr, const int &port, const int &af, const int &sock)
{
	this->port = port;
	this->af = af;
	this->sock = sock;
	strcpy(this->addr, addr);
	this->errcode = 0;
	this->errmsg = nullptr;
	if (!CreateSocket(this->fd, af, sock))
		SocketError();
}

Socket::~Socket()
{
	this->addr[0] = 0;
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
		setlocale(LC_ALL, "");
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
Client::Client() : Socket(AF_INET, SOCK_STREAM) {}
Client::Client(const char *addr, const int &port) : Socket(addr, port, AF_INET, SOCK_STREAM) {}
Client::~Client() {}
Client::Client(const Client &c) : Socket(c) {}
Client::Client(Client &&c) : Socket((Client &&) c) {}
Client &Client::operator=(const Client &c)
{
	Socket::operator=(c);
	return *this;
}

Client &Client::operator=(Client &&c)
{
	Socket::operator=((Client &&) c);
	return *this;
}

bool Client::Connect(const char *addr, const int &port)
{
	this->port = port;
	strcpy(this->addr, addr);
	if (!CreateSocket(this->fd, AF_INET, SOCK_STREAM))
		return false;
	return this->Connect();
}

Server::Server(const char *addr, const int &port) : Socket(addr, port, AF_INET, SOCK_STREAM) {}
Server::~Server() {}
bool Client::Connect()
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->port);
	addr.sin_addr.s_addr = inet_addr(this->addr);
	int ret = connect(fd, (sockaddr *)&addr, sizeof(addr));
	if (ret)
	{
		SocketError();
		return false;
	}
	return true;
}

bool Server::Listen()
{
	if (this->errcode)
		return false;
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(this->port);
	addr.sin_addr.s_addr = inet_addr(this->addr);
	int ret = bind(fd, (sockaddr *)&addr, sizeof(addr));
	if (ret)
	{
		SocketError();
		return false;
	}
	ret = listen(fd, 10);
	if (ret)
	{
		SocketError();
		return false;
	}
	return true;
}

bool Server::Accept(Socket &socket)
{
	sockaddr_in client;
	socklen_t addrlen = sizeof(client);
	int fd;
	if ((fd = accept(this->fd, (sockaddr *)&client, &addrlen)) == SOCKET_ERROR)
	{
		SocketError();
		return false;
	}
	int port = ntohs(client.sin_port);
	char addr[16];
	NetToHost(&client.sin_addr.s_addr, addr);
	socket = Socket(fd, addr, port, af, sock);
	return true;
}

#ifdef _WIN32

#else

#endif
