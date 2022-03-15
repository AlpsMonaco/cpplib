#include "network.hpp"

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

int main(int argc, char **argv)
{
	network::tcp::Server s("127.0.0.1", 61111);
	if (!s.Listen())
	{
		Println(s.Errno());
		return 1;
	}
}