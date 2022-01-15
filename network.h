#pragma once
#ifndef __NETWORK_H
#define __NETWORK_H

namespace network
{
	bool IsDomain(const char *ip);
	void NetToHost(const void *net, char dst[16]);
	bool DNSResolve(const char *name, char *dst);

	class Socket
	{
	public:
		Socket();
		Socket(Socket &&s);
		Socket(const Socket &s);
		Socket(const int &af, const int &sock);
		Socket(const char *addr, const int &port, const int &af, const int &sock);
		Socket(int fd, const char *addr, const int &port, const int &af, const int &sock);
		virtual ~Socket();

		Socket &operator=(const Socket &s);
		Socket &operator=(Socket &&s);

		int Send(const char *buf, const int &bufSize);
		int Recv(char *buf, const int &bufSize);
		int Close();
		const char *GetAddr();
		int GetPort();

		int errcode;
		char *errmsg;

	protected:
		void SocketError();
		int af;
		int sock;
		char addr[16];
		int port;
		int fd;
	};

	namespace tcp
	{
		class Client : public Socket
		{
		public:
			Client();
			Client(const Client &c);
			Client(Client &&c);
			Client(const char *addr, const int &port);
			~Client();

			Client &operator=(const Client &c);
			Client &operator=(Client &&c);

			bool Connect();
			bool Connect(const char *addr, const int &port);
		};

		class Server : public Socket
		{
		public:
			Server(const char *addr, const int &port);
			~Server();

			bool Listen();
			bool Accept(Socket &socket);
		};
	}

}
#endif
