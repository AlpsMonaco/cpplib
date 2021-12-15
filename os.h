#pragma once
#ifndef __OS_H
#define __OS_H
#include <stdio.h>
#include <string>
#include <io.h>
#include <direct.h>

#ifndef ExportDLL
#define ExportDLL __declspec(dllexport)
#endif

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
		std::string result;
	};

	ExportDLL ExecuteResult Execute(const char *cmd);
	ExportDLL ExecuteResult Execute(const std::string &cmd);
	ExportDLL bool IsDirExist(const char *dirPath);
	ExportDLL bool IsDirExist(std::string &dirPath);
	ExportDLL bool IsFileExist(const char *filePath);
	ExportDLL bool IsFileExist(std::string &filePath);
	ExportDLL bool MakeDir(const char *dirPath);
	ExportDLL bool MakeDir(std::string &dirPath);
}

#endif