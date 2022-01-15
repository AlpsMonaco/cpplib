// #define __BUILD_LIB_MODE 1
#ifdef __BUILD_LIB_MODE
#ifndef __MARCO_PRINTLN
#define __MARCO_PRINTLN
#include <iostream>
#define Println(word)                   \
	do                                  \
	{                                   \
		std::cout << word << std::endl; \
	} while (0)
#endif

#include <os.h>
#include <stringext.h>
#include <filepath.h>
#include <cstdio>

template <typename T>
void ErrorHandle(T t)
{
	Println(t);
}

class BuildLIB
{
public:
	static void Start()
	{
		std::string filePath = ".";
		std::string keyword = ".cpp";
		std::vector<std::string> fileList = BuildLIB::GetFileList(filePath, keyword, std::vector<std::string>{"main.cpp"});
		BuildLIB::CompileCppFiles(fileList);
		keyword = ".obj";
		fileList = BuildLIB::GetFileList(filePath, keyword, std::vector<std::string>{"main.obj"});
		BuildLIB::CreateStaticLIB(fileList);
		BuildLIB::Clean();
	}

	static std::vector<std::string> GetFileList(std::string filePath, const std::string &keyword, const std::vector<std::string> &excludeFileList)
	{
		if (filePath[filePath.length() - 1] != '/' || filePath[filePath.length() - 1] != '\\')
			filePath += "\\";

		std::vector<std::string> v;
		std::string cmd = "dir /b " + std::string(filePath);
		os::ExecuteResult result = os::Execute(cmd);
		if (result.code)
		{
			ErrorHandle(result.output);
			return v;
		}

		stringext::TrimSpace(result.output);
		stringext::ReplaceAll(result.output, "\r\n", "\n");
		v = stringext::Split(result.output, "\n");
		std::vector<std::string> resultFileList;
		for (auto it = v.begin(); it < v.end(); it++)
		{
			if (it->find(keyword) != std::string::npos)
			{
				if ([&it, &excludeFileList]() -> bool
					{
						for (auto v : excludeFileList)
						{
							if (v == *it)
								return true;
						}
						return false;
					}())
					continue;
				resultFileList.push_back(filePath + *it);
			}
		}
		return resultFileList;
	}

	static void CompileCppFiles(std::vector<std::string> &cppFileList)
	{
		std::string s, msg;
		os::ExecuteResult result;
		for (auto file : cppFileList)
		{
			s = "cl /c /EHsc /Ox /nologo " + file;
			result = os::Execute(s.c_str());
			if (result.code)
				msg = "compile " + file + " failed";
			else
				msg = "compile " + file + " success";
			Println(msg);
		}
	}

	static void CreateStaticLIB(std::vector<std::string> &objFileList)
	{
		os::MakeDir("./static");
		os::ExecuteResult result;
		std::string cmd, msg;
		for (auto objFilePath : objFileList)
		{
			std::string objName = filepath::FileBaseName(objFilePath);
			cmd = "lib /out:static/" + objName + ".lib " + objFilePath;
			result = os::Execute(cmd);
			msg = "lib " + objFilePath;
			if (result.code)
				msg += " failed";
			else
				msg += " success";
			Println(msg);
		}
	}

	static void Clean()
	{
		const std::vector<std::string> cleanExtList = std::vector<std::string>{".obj", ".ilk", ".pdb", ".o"};
		os::ExecuteResult result = os::Execute("dir /b");
		if (result.code)
		{
			ErrorHandle(result.output);
			return;
		}
		stringext::ReplaceAll(result.output, "\r\n", "\n");
		std::vector<std::string> resultVector = stringext::Split(result.output, "\n");
		for (auto it : resultVector)
		{
			for (auto extIt : cleanExtList)
			{
				if (it.find(extIt) != std::string::npos)
				{
					std::cout << "delete " + it << std::endl;
					os::DeleteFile(it.c_str());
				}
			}
		}
	}
};

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		if (strcmp(argv[1], "-c") == 0)
		{
			BuildLIB::Clean();
			return 0;
		}
	}
	BuildLIB::Start();
	return 0;
}

#else

#ifndef __MARCO_PRINTLN
#define __MARCO_PRINTLN
#include <iostream>
#define Println(word)                   \
	do                                  \
	{                                   \
		std::cout << word << std::endl; \
	} while (0)
#endif

#include "network.h"
#include "thread"
const int port = 33123;
const char *addr = "127.0.0.1";

void ServerMethod()
{
	network::tcp::Server s(addr, port);
	if (!s.Listen())
	{
		Println(s.errmsg);
		Println("server listen failed");
		return;
	}
	network::Socket c;
	if (!s.Accept(c))
	{
		Println(s.errmsg);
		Println("Accept failed");
		return;
	}
	char buf[128];
	int size = c.Send("Hello Client", 13);
	if (size == -1)
	{
		Println("Send failed");
		return;
	}
	size = c.Recv(buf, 128);
	if (size == -1)
	{
		Println("Recv failed");
		return;
	}
	Println(buf);
	c.Close();
	s.Close();
}

void ClientMethod()
{
	network::tcp::Client c;
	if (!c.Connect(addr, port))
	{
		Println(c.errmsg);
		Println("connect failed");
		return;
	}
	char buf[128];
	int size = c.Recv(buf, 128);
	if (size == -1)
	{
		Println("Recv failed");
		return;
	}
	Println(buf);
	size = c.Send("Hello Server", 13);
	if (size == -1)
	{
		Println("Send failed");
		return;
	}
	c.Close();
}

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

void NetToHost(const void *net, char dst[16])
{
	unsigned char *temp = (unsigned char *)net;
	for (int i = 0; i < 4; i++)
	{
		sprintf(dst, "%u", temp[i]);
		do
		{
		} while (*(++dst) != 0);

		if (i != 3)
		{
			*dst = '.';
			dst++;
		}
	}
}

bool IsDomain(const char *ip)
{
	char c;
	while ((c = *ip++) != 0)
	{
		if (c < 46 || c > 57)
			return true;
	}
	return false;
}

bool DNSResolve(const char *name, char *dst)
{
	hostent *host = gethostbyname(name);
	if (!host)
		return false;
	if (host->h_addr_list[0])
		NetToHost(host->h_addr_list[0], dst);
	return true;
}

#pragma comment(lib, "ws2_32.lib")
int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	const char *ip = "idle-as.hotgamehl.com";
	char dst[16] = "";
	if (!DNSResolve(ip, dst))
	{
		return 1;
	}
	Println(dst);
	system("pause");
	return 0;
}

#endif