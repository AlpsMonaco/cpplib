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

#include <iostream>
#include <log.hpp>
#include <utility>

struct Test
{
	int i;
	char buf[64];
	Test()
	{
		i = 0;
		memset(buf, 0, 64);
	}
};

int main()
{
	Test t1;
	t1.buf[0] = 'a';

	Test t2 = t1;
	memset(&t1, 0, sizeof(t1));

	Println(t2.i);
	Println(t2.buf);

	// logs::Log("info").Write("info");
	// logs::Log("err").Write("err");
	// logs::Logger("info") << "1"
	// 					 << "2";
	// logs::Logger("info").Write("3");
	// logs::Logger("err").Write("err");
	// logs::Logger logger("test");
	// logs::LoggerStream<logs::Logger> ls(&logger);
	// logs::LoggerStream<logs::Logger> ls2(std::move(ls));
	// logs::Logger infologger("info");
	// logs::Logger infologger2(std::move(infologger));
	// infologger2.Write("log2");
	// infologger.Write("log");
}