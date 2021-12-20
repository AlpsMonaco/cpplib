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
#include <strings.h>
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
		strings::TrimSpace(result.output);
		strings::ReplaceString(result.output, "\r\n", "\n");
		v = strings::SplitString(result.output, "\n");
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
		const std::vector<std::string> cleanExtList = std::vector<std::string>{".obj", ".ilk", ".pdb"};
		os::ExecuteResult result = os::Execute("dir /b");
		if (result.code)
		{
			ErrorHandle(result.output);
			return;
		}
		strings::ReplaceString(result.output, "\r\n", "\n");
		std::vector<std::string> resultVector = strings::SplitString(result.output, "\n");
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
	BuildLIB::Start();
}