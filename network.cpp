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

void (*GlobalSocketInit)() = []() -> void
{ WSAInit(); };
#else
void (*GlobalSocketInit)() = []() -> void {};
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

network::Socket::Socket(network::Socket &&rhs) : addr(rhs.addr), sockType(rhs.sockType), fd(rhs.fd)
{
	GlobalSocketInit();
	rhs.addr.sin_port = 0;
	rhs.addr.sin_family = 0;
	rhs.addr.sin_zero[0] = 0;
	rhs.addr.sin_addr.s_addr = 0;
	rhs.fd = 0;
}

network::Socket::Socket() : addr({0, 0, {0}}), sockType(0), fd(0) { GlobalSocketInit(); }
network::Socket::Socket(int afType, int sockType, const char *addr, int port) : sockType(sockType), fd(0)
{
	GlobalSocketInit();
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

void network::Socket::Close()
{
	closesocket(this->fd);
	this->fd = 0;
}

bool network::Socket::Send(const char *buf, int bufSize) { return send(this->fd, buf, bufSize, 0) != SOCKET_ERROR; }
bool network::Socket::Recv(char *buf, int bufSize) { return recv(fd, buf, bufSize, 0) != SOCKET_ERROR; }
int network::Socket::Errno() { return WSAGetLastError(); }
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

void network::Socket::SetSocketFd(SocketFd fd) { this->fd = fd; }
void network::Socket::SetSockType(int sockType) { this->sockType = sockType; }

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

bool network::tcp::Server::Accept(Socket &socket)
{
	int addrlen = SOCKADDR_IN_SIZE;
	int fd = accept(this->fd, (sockaddr *)socket.GetSockAddr(), &addrlen);
	if (fd == INVALID_SOCKET)
		return false;
	socket.SetSockType(SOCK_STREAM);
	socket.SetSocketFd(fd);
	return true;
}