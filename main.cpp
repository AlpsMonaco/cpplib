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
		stringext::ReplaceString(result.output, "\r\n", "\n");
		v = stringext::SplitString(result.output, "\n");
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
		stringext::ReplaceString(result.output, "\r\n", "\n");
		std::vector<std::string> resultVector = stringext::SplitString(result.output, "\n");
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

int BeginBuildLIB(int argc, char **argv)
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

#endif

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
const int port = 54444;
const char *addr = "127.0.0.1";

void ServerMethod()
{
	network::tcp::Server s(addr, port);
	if (!s.Listen())
	{
		Println("server listen failed");
		return;
	}
	network::Socket c;
	if (!s.Accept(c))
	{
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

int main(int argc, char **argv)
{
	std::thread serverThread(ServerMethod);
	std::this_thread::sleep_for(std::chrono::seconds(3));
	std::thread clientThread(ClientMethod);

	serverThread.join();
	clientThread.join();
	return 0;
}