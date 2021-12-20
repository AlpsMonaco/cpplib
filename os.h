#pragma once
#ifndef __OS_H
#define __OS_H
#include <stdio.h>
#include <string>
#include <io.h>
#include <direct.h>
#include <vector>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace os
{
	struct ExecuteResult
	{
	public:
		int code;
		std::string output;
	};

	ExecuteResult Execute(const char *cmd);
	ExecuteResult Execute(const std::string &cmd);
	bool IsDirExist(const char *dirPath);
	bool IsDirExist(std::string &dirPath);
	bool IsFileExist(const char *filePath);
	bool IsFileExist(std::string &filePath);
	bool MakeDir(const char *dirPath);
	bool MakeDir(std::string &dirPath);
	bool DeleteFile(std::string &filePath);
	bool DeleteFile(const char *filePath);
}

#endif