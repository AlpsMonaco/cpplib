#pragma once
#ifndef __OS_H
#define __OS_H
#include <string>
#include <iostream>

#ifdef _WIN32
#include <io.h>
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

	bool MakeDir(const char *dirPath);
	ExecuteResult Execute(const char *cmd);
	ExecuteResult Execute(const std::string &cmd);
	inline bool IsPathExist(const char *path) { return access(path, 0) == 0; }
	inline bool IsDirExist(const char *dirPath) { return IsPathExist(dirPath); }
	inline bool IsDirExist(const std::string &dirPath) { return IsPathExist(dirPath.c_str()); }
	inline bool IsFileExist(const char *filePath) { return IsPathExist(filePath); }
	inline bool IsFileExist(const std::string &filePath) { return IsPathExist(filePath.c_str()); }
	inline bool MakeDir(const std::string &dirPath) { return MakeDir(dirPath.c_str()); }
	inline bool DeleteFile(const std::string &filePath) { return DeleteFile(filePath.c_str()); }
	inline bool DeleteFile(const char *filePath) { return remove(filePath) == 0; }
}

#endif