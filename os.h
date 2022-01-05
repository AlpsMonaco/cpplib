#pragma once
#ifndef __OS_H
#define __OS_H
#include <stdio.h>
#include <string>
#include <iostream>
#include <vector>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#define popen _popen
#define pclose _pclose
#else
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#define mkdir(dst) mkdir(dst, 0755)
#endif

namespace os
{
	struct ExecuteResult
	{
	public:
		int code;
		std::string output;
		friend std::ostream &operator<<(std::ostream &os, const ExecuteResult &result)
		{
			os << result.code;
			if (result.output != "")
				os << std::endl
				   << result.output;
			return os;
		}
	};

	ExecuteResult Execute(const char *cmd);
	ExecuteResult Execute(const std::string &cmd);
	bool IsDirExist(const char *dirPath);
	bool IsDirExist(const std::string &dirPath);
	bool IsFileExist(const char *filePath);
	bool IsFileExist(const std::string &filePath);
	bool MakeDir(const char *dirPath);
	bool MakeDir(const std::string &dirPath);
	bool DeleteFile(const std::string &filePath);
	bool DeleteFile(const char *filePath);
}

#endif