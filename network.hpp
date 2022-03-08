#ifndef __NETWORK_H
#define __NETWORK_H

#ifdef _WIN32

#include <winsock2.h>
#pragma comment(lib, "WS2_32.Lib")

#else

#include <netinet/in.h>
using SOCKET = int;

#endif

#include <map>
#include <iostream>

namespace network
{
	using SocketFd = SOCKET;
	class Socket
	{
	public:
		Socket();
		Socket(int afType, int sockType, const char *addr = "0.0.0.0", int port = 0, int fd = 0);
		Socket(int sockType, const sockaddr_in *pAddr, int fd = 0);
		Socket(const Socket &rhs);
		Socket(Socket &&rhs);

		Socket &operator=(const Socket &rhs);
		Socket &operator=(Socket &&rhs);

		int GetPort();
		void GetAddr(char *dst);
		const sockaddr_in *GetSockAddr();
		bool Close();
		int Errno();
		int Send(const char *buf, int size);
		int Recv(char *buf, int size);

		int GetPort() const;
		void GetAddr(char *dst) const;
		const sockaddr_in *GetSockAddr() const;
		int Send(const char *buf, int size) const;
		int Recv(char *buf, int size) const;

	protected:
		sockaddr_in addr;
		int sockType;
		SocketFd fd;

		bool CreateSocket();
	};

	namespace tcp
	{
		class Client : public Socket
		{
		public:
			Client();
			Client(const char *addr, int port);

			bool Connect();
			bool Connect(const char *addr, int port);
		};

		class Server : public Socket
		{
		public:
			using ServerCallback = void (*)(Socket &socket);

			Server(const char *addr, int port, int buffersize = 1024);
			~Server();

			void SetOnNewConnection(bool (*onNewConnection)(const Socket &socket));
			void SetOnNewData(bool (*onNewData)(const Socket &socket, char *data, int recvsize));
			void SetOnConnectionClose(void (*onConnectionClose)(const Socket &socket));
			void SetOnError(void (*onError)(const char *message));
			bool Listen();
			void Begin();

		protected:
			using SocketMap = std::map<SocketFd, Socket>;

			// return false if you want to close this connection.
			bool (*OnNewConnection)(const Socket &socket);
			// return false if you want to close this connection.
			bool (*OnNewData)(const Socket &socket, char *data, int recvsize);
			void (*OnConnectionClose)(const Socket &socket);
			void (*OnError)(const char *);

			static bool DefaultOnNewConnection(const Socket &socket);
			static bool DefaultOnNewData(const Socket &socket, char *data, int recvsize);
			static void DefaultOnConnectionClose(const Socket &socket);
			static void DefaultOnError(const char *msg);

			Socket &GetSocket(SocketFd cfd);
			void AddSocket(SocketFd cfd, const sockaddr_in &sockaddr);
			void RemoveSocket(SocketFd cfd);

			SocketMap socketmap;
			FD_SET socketset;
			char *buffer;
			int buffersize;
		};
	}
}

namespace network
{
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

	void (*Init)() = []() -> void
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

	void (*Init)() = []() -> void {};
	inline void itoa(int value, char *buf, int unuse) { sprintf(buf, "%d", value); }
	int (*closesocket)(int) = close;
	inline int GetErrno() { return errno; }

#endif

	Socket &Socket::operator=(const Socket &rhs)
	{
		this->addr = rhs.addr;
		this->sockType = rhs.sockType;
		this->fd = rhs.fd;
		return *this;
	}

	Socket &Socket::operator=(Socket &&rhs)
	{
		*this = static_cast<const Socket &>(rhs);
		rhs.addr.sin_port = 0;
		rhs.addr.sin_family = 0;
		rhs.addr.sin_zero[0] = 0;
		rhs.addr.sin_addr.s_addr = 0;
		rhs.fd = 0;
		return *this;
	}

	Socket::Socket(const Socket &rhs) : addr(rhs.addr), sockType(rhs.sockType), fd(rhs.fd) {}
	Socket::Socket(Socket &&rhs) : addr(rhs.addr), sockType(rhs.sockType), fd(rhs.fd)
	{
		rhs.addr.sin_port = 0;
		rhs.addr.sin_family = 0;
		rhs.addr.sin_zero[0] = 0;
		rhs.addr.sin_addr.s_addr = 0;
		rhs.fd = 0;
	}

	Socket::Socket() : addr{0, 0, 0, {0}}, sockType(0), fd(0) { Init(); }
	Socket::Socket(int sockType, const sockaddr_in *pAddr, int fd) : sockType(sockType), fd(fd), addr(*pAddr) { Init(); }
	Socket::Socket(int afType, int sockType, const char *addr, int port, int fd) : sockType(sockType),
																				   fd(fd), addr{0, 0, 0, {0}}
	{
		this->addr.sin_family = afType;
		this->addr.sin_addr.s_addr = inet_addr(addr);
		this->addr.sin_port = htons(port);
		Init();
	}

	void Socket::GetAddr(char *dst) const
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

	int Socket::GetPort() const { return ntohs(this->addr.sin_port); }

	bool Socket::CreateSocket()
	{
		this->fd = socket(this->addr.sin_family, this->sockType, 0);
		return this->fd != INVALID_SOCKET;
	}

	bool Socket::Close()
	{
		int status = closesocket(this->fd);
		this->fd = 0;
		return status != SOCKET_ERROR;
	}

	int Socket::Send(const char *buf, int bufSize) const { return send(this->fd, buf, bufSize, 0); }
	int Socket::Recv(char *buf, int bufSize) const { return recv(fd, buf, bufSize, 0); }
	int Socket::Errno() { return GetErrno(); }
	const sockaddr_in *Socket::GetSockAddr() const { return const_cast<const sockaddr_in *>(&this->addr); }

	tcp::Client::Client() : Socket(AF_INET, SOCK_STREAM) {}
	tcp::Client::Client(const char *addr, int port) : Socket(AF_INET, SOCK_STREAM, addr, port) {}

	bool tcp::Client::Connect()
	{
		if (!this->CreateSocket())
			return false;
		return connect(this->fd, (sockaddr *)this->GetSockAddr(), SOCKADDR_IN_SIZE) != SOCKET_ERROR;
	}

	bool tcp::Client::Connect(const char *addr, int port)
	{
		this->addr.sin_port = htons(port);
		this->addr.sin_addr.s_addr = inet_addr(addr);
		return this->Connect();
	}

	tcp::Server::Server(const char *addr, int port, int buffersize) : Socket(AF_INET, SOCK_STREAM, addr, port),
																	  OnNewConnection(nullptr),
																	  OnConnectionClose(nullptr),
																	  OnNewData(nullptr),
																	  OnError(nullptr),
																	  socketmap(),
																	  socketset(),
																	  buffer(nullptr),
																	  buffersize(0)
	{
		FD_ZERO(&this->socketset);
		this->buffersize = buffersize;
		this->buffer = (char *)malloc(this->buffersize);
	}

	tcp::Server::~Server()
	{
		if (this->buffer != nullptr)
			free(this->buffer);
	}

	bool tcp::Server::Listen()
	{
		if (!this->CreateSocket())
			return false;
		u_long arg = 1;
		if (ioctlsocket(this->fd, FIONBIO, &arg))
			return false;
		if (bind(this->fd, (sockaddr *)&this->addr, SOCKADDR_IN_SIZE) == SOCKET_ERROR)
			return false;
		if (listen(this->fd, 10) == SOCKET_ERROR)
			return false;
		return true;
	}

	void tcp::Server::Begin()
	{
		if (this->OnNewConnection == nullptr)
			this->OnNewConnection = DefaultOnNewConnection;
		if (this->OnNewData == nullptr)
			this->OnNewData = DefaultOnNewData;
		if (this->OnConnectionClose == nullptr)
			this->OnConnectionClose = DefaultOnConnectionClose;
		if (this->OnError == nullptr)
			this->OnError = DefaultOnError;

		FD_SET(this->fd, &this->socketset);
		fd_set readableset;
		int status, recvsize;
		unsigned int i;
		sockaddr_in clientAddr;
		SocketFd cfd;
		Socket &rsocket = Socket();
		int addrlen = sizeof(clientAddr);
		for (;;)
		{
			readableset = this->socketset;
			status = select(NULL, &readableset, NULL, NULL, NULL);
			if (status == SOCKET_ERROR)
			{
				this->OnError("socket error on I/O select");
				return;
			}
			for (i = 0; i < readableset.fd_count; i++)
			{
				if (readableset.fd_array[i] == this->fd)
				{
					cfd = accept(this->fd, (sockaddr *)&clientAddr, &addrlen);
					if (cfd == SOCKET_ERROR)
					{
						this->OnError("accept socket failed");
						return;
					}
					this->AddSocket(cfd, clientAddr);
					rsocket = this->GetSocket(cfd);
					if (!this->OnNewConnection(rsocket))
					{
						rsocket.Close();
						this->RemoveSocket(cfd);
					}
				}
				else
				{
					cfd = readableset.fd_array[i];
					rsocket = GetSocket(cfd);
					recvsize = rsocket.Recv(this->buffer, this->buffersize);
					if (recvsize > 0)
					{
						if (!this->OnNewData(rsocket, this->buffer, recvsize))
						{
							rsocket.Close();
							this->RemoveSocket(cfd);
						}
					}
					else
					{
						this->OnConnectionClose(rsocket);
						this->RemoveSocket(readableset.fd_array[i]);
					}
				}
			}
		}
	}

	void tcp::Server::AddSocket(SocketFd cfd, const sockaddr_in &sockaddr)
	{
		FD_SET(cfd, &this->socketset);
		this->socketmap.emplace(cfd, Socket(SOCK_STREAM, &sockaddr, cfd));
	}

	void tcp::Server::RemoveSocket(SocketFd cfd)
	{
		FD_CLR(cfd, &this->socketset);
		this->socketmap.erase(cfd);
	}

	Socket &tcp::Server::GetSocket(SocketFd cfd) { return socketmap.at(cfd); }

	// return false if you want to close this connection.
	void tcp::Server::SetOnNewConnection(bool (*onNewConnection)(const Socket &socket)) { this->OnNewConnection = onNewConnection; }
	// return false if you want to close this connection.
	void tcp::Server::SetOnNewData(bool (*onNewData)(const Socket &socket, char *data, int recvsize)) { this->OnNewData = onNewData; }
	void tcp::Server::SetOnConnectionClose(void (*onConnectionClose)(const Socket &socket)) { this->OnConnectionClose = onConnectionClose; }
	void tcp::Server::SetOnError(void (*onError)(const char *message)) { this->OnError = onError; }

	bool tcp::Server::DefaultOnNewConnection(const Socket &socket)
	{
		static char buf[64];
		socket.GetAddr(buf);
		std::cout << "new connection: " << buf << ':' << socket.GetPort() << std::endl;
		return true;
	}

	bool tcp::Server::DefaultOnNewData(const Socket &socket, char *data, int recvsize)
	{
		static char buf[64];
		socket.GetAddr(buf);
		data[recvsize] = 0;
		std::cout << "new data received from " << buf << ':' << socket.GetPort() << std::endl
				  << data << std::endl;
		return true;
	}

	void tcp::Server::DefaultOnConnectionClose(const Socket &socket)
	{
		static char buf[64];
		socket.GetAddr(buf);
		std::cout << "client closed its connection: " << buf << ':' << socket.GetPort() << std::endl;
	}

	void tcp::Server::DefaultOnError(const char *msg) { std::cout << msg << std::endl; }

	int network::Socket::GetPort() { return const_cast<const network::Socket &>(*this).GetPort(); }
	void network::Socket::GetAddr(char *dst) { return const_cast<const network::Socket &>(*this).GetAddr(dst); }
	const sockaddr_in *network::Socket::GetSockAddr() { return const_cast<const network::Socket &>(*this).GetSockAddr(); }
	int network::Socket::Send(const char *buf, int size) { return const_cast<const network::Socket &>(*this).Send(buf, size); }
	int network::Socket::Recv(char *buf, int size) { return const_cast<const network::Socket &>(*this).Recv(buf, size); }
}

#endif