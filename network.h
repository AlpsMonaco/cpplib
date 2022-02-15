#ifndef __NETWORK_H
#define __NETWORK_H

#ifdef _WIN32

#include <winsock2.h>
#pragma comment(lib, "WS2_32.Lib")

#else

#endif

namespace network
{
	using SocketFd = SOCKET;
	class Socket
	{
	public:
		Socket();
		Socket(int afType, int sockType, const char *addr = "0.0.0.0", int port = 0);

		Socket(Socket &&rhs);
		Socket &operator=(Socket &&rhs);

		Socket &operator=(Socket &rhs);

		int GetPort();
		void GetAddr(char *dst);
		sockaddr_in *GetSockAddr();
		void SetSocketFd(SocketFd fd);
		void SetSockType(int sockType);

		const sockaddr_in *GetSockAddr() const;

		void Close();
		int Errno();

		bool Send(const char *buf, int size);
		bool Recv(char *buf, int size);

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
			Server(const char *addr, int port);

			bool Listen();
			bool Accept(Socket &socket);
		};
	}
}

#endif