#pragma once
#ifndef __NETWORK_H
#define __NETWORK_H

namespace network
{
	class Socket
	{
	public:
		Socket(const char *addr, const int &port, const int &af, const int &sock);
		virtual ~Socket();
		virtual int Send(const char *buf, const int &bufSize) = 0;
		virtual int Recv(char *buf, const int &bufSize) = 0;
		virtual int Close() = 0;
		const char *Error();
		int Errno();

	protected:
		void SetError(const int &errcode, const char *errmsg);
		int af;
		int sock;
		char *addr;
		int port;
		char *errmsg;
		int errcode;
		int fd;
	};

	namespace tcp
	{
		class Client : public Socket
		{
		public:
			Client(const char *addr, const int &port);
			~Client();
			bool Connect();
			int Send(const char *buf, const int &bufSize);
			int Recv(char *buf, const int &bufSize);
			int Close();
		};

		class Server : public Socket
		{
		public:
			Server(const char *addr, const int &port);
			~Server();
			bool Listen(const char *addr, const int &port);
			int Send(const char *buf, const int &bufSize);
			int Recv(char *buf, const int &bufSize);
			int Close();
		};
	}

}
#endif
