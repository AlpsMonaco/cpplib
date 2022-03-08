#ifndef __PRINT_MULTI_ARGS__
#define __PRINT_MULTI_ARGS__
#include <iostream>

template <typename T>
inline void Print(T t) { std::cout << t; }

template <typename T, typename... Args>
void Print(T t, Args... args)
{
	std::cout << t << " ";
	Print(args...);
}

template <typename... Args>
void Println(Args... args)
{
	Print(args...);
	std::cout << std::endl;
}

#endif

#include "network.hpp"
#include <thread>

int main(int argc, char **argv)
{
	// const network::Socket s;
	network::tcp::Server server("127.0.0.1", 65001);
	if (!server.Listen())
	{
		Println(server.Errno());
		return 1;
	}
	// server.SetOnNewData([](const network::Socket &socket, char *data, int recvsize) -> bool
	// 					{
	// 	data[recvsize] = 0;
	// 	Println(data);
	// 	 return false; });
	// std::thread t([&server]() -> void
	// 			  { std::this_thread::sleep_for(std::chrono::seconds(10));server.Close(); });
	// t.detach();
	server.Begin();
	Println(server.Errno());
}