#pragma once
#ifndef __NETWORK_H
#define __NETWORK_H

namespace network
{
	class Socket
	{
	public:
		Socket(int fd, const char *addr, const int &port);
		Socket(const char *addr, const int &port, const int &af, const int &sock);
		virtual ~Socket();
		int Send(const char *buf, const int &bufSize);
		int Recv(char *buf, const int &bufSize);
		int Close();
		const char *GetAddr();
		int GetPort();
		int errcode;
		char *errmsg;

	protected:
		void SetError(const int &errcode, const char *errmsg);
		int af;
		int sock;
		char *addr;
		int port;
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
		};

		class Server : public Socket
		{
		public:
			Server(const char *addr, const int &port);
			~Server();
			bool Listen();
			Socket Accept();
		};
	}

}
#endif
