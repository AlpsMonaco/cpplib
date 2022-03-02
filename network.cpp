#include "network.h"

constexpr int SOCKADDR_IN_SIZE = sizeof(sockaddr_in);

#ifdef _WIN32

struct WSAManager
{
	WSAManager()
	{
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);
	}
	~WSAManager() { WSACleanup(); }
};

WSAManager &WSAInit()
{
	static WSAManager wsamanager;
	return wsamanager;
}

void (*SysSocketInit)() = []() -> void
{ WSAInit(); };
inline int GetErrno() { return WSAGetLastError(); }
using socklen_t = int;

#else

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1

void (*SysSocketInit)() = []() -> void {};
inline void itoa(int value, char *buf, int unuse) { sprintf(buf, "%d", value); }
int (*closesocket)(int) = close;
inline int GetErrno() { return errno; }

#endif

network::Socket &network::Socket::operator=(Socket &rhs)
{
	this->addr = rhs.addr;
	this->sockType = rhs.sockType;
	this->fd = rhs.fd;
	return *this;
}

network::Socket &network::Socket::operator=(Socket &&rhs)
{
	*this = static_cast<Socket &>(rhs);
	rhs.addr.sin_port = 0;
	rhs.addr.sin_family = 0;
	rhs.addr.sin_zero[0] = 0;
	rhs.addr.sin_addr.s_addr = 0;
	rhs.fd = 0;
	return *this;
}

network::Socket::Socket(Socket &rhs) : addr(rhs.addr), sockType(rhs.sockType), fd(rhs.fd) { SysSocketInit(); }
network::Socket::Socket(network::Socket &&rhs) : addr(rhs.addr), sockType(rhs.sockType), fd(rhs.fd)
{
	SysSocketInit();
	rhs.addr.sin_port = 0;
	rhs.addr.sin_family = 0;
	rhs.addr.sin_zero[0] = 0;
	rhs.addr.sin_addr.s_addr = 0;
	rhs.fd = 0;
}

network::Socket::Socket() : addr({0, 0, {0}}), sockType(0), fd(0) { SysSocketInit(); }
network::Socket::Socket(int afType, int sockType, const char *addr, int port) : sockType(sockType), fd(0)
{
	SysSocketInit();
	this->addr.sin_family = afType;
	this->addr.sin_port = htons(port);
	this->addr.sin_addr.s_addr = inet_addr(addr);
}

void network::Socket::GetAddr(char *dst)
{
	for (int i = 0; i < 4; i++)
	{
		itoa((this->addr.sin_addr.s_addr & 0xFF << (8 * i)) >> (8 * i), dst, 10);
		size_t size = strlen(dst);
		if (i != 3)
		{
			dst[size] = '.';
			dst += size + 1;
			continue;
		}
		dst[size] = '\0';
		break;
	}
}

int network::Socket::GetPort() { return ntohs(this->addr.sin_port); }

bool network::Socket::CreateSocket()
{
	this->fd = socket(this->addr.sin_family, this->sockType, 0);
	return this->fd != INVALID_SOCKET;
}

bool network::Socket::Close()
{
	int status = closesocket(this->fd);
	this->fd = 0;
	return status != SOCKET_ERROR;
}

int network::Socket::Send(const char *buf, int bufSize) { return send(this->fd, buf, bufSize, 0); }
int network::Socket::Recv(char *buf, int bufSize) { return recv(fd, buf, bufSize, 0); }
int network::Socket::Errno() { return GetErrno(); }
const sockaddr_in *network::Socket::GetSockAddr() const { return &this->addr; }
sockaddr_in *network::Socket::GetSockAddr() { return &this->addr; }

network::tcp::Client::Client() : Socket(AF_INET, SOCK_STREAM) {}
network::tcp::Client::Client(const char *addr, int port) : Socket(AF_INET, SOCK_STREAM, addr, port) {}

bool network::tcp::Client::Connect()
{
	if (!this->CreateSocket())
		return false;
	return connect(this->fd, (sockaddr *)this->GetSockAddr(), SOCKADDR_IN_SIZE) != SOCKET_ERROR;
}

bool network::tcp::Client::Connect(const char *addr, int port)
{
	this->addr.sin_port = htons(port);
	this->addr.sin_addr.s_addr = inet_addr(addr);
	return this->Connect();
}

network::tcp::Server::Server(const char *addr, int port) : Socket(AF_INET, SOCK_STREAM, addr, port) {}
bool network::tcp::Server::Listen()
{
	if (!this->CreateSocket())
		return false;
	if (bind(this->fd, (sockaddr *)&this->addr, SOCKADDR_IN_SIZE) == SOCKET_ERROR)
		return false;
	if (listen(this->fd, 10) == SOCKET_ERROR)
		return false;
	return true;
}

void network::Socket::AcquireSocket(network::Socket &socket, SocketFd fd, int sockType)
{
	socket.fd = fd;
	socket.sockType = sockType;
}

bool network::tcp::Server::Accept(Socket &socket)
{
	socklen_t addrlen = SOCKADDR_IN_SIZE;
	int fd = accept(this->fd, (sockaddr *)socket.GetSockAddr(), &addrlen);
	if (fd == INVALID_SOCKET)
		return false;
	network::Socket::AcquireSocket(socket, fd, SOCK_STREAM);
	return true;
}