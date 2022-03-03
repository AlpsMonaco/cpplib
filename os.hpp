#pragma once
#ifndef __OS_H
#define __OS_H
#include <string>
#include <vector>
#include <iostream>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#include <sys/stat.h>
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
	std::vector<std::string> ListDir(const char *path);
	std::vector<std::string> ListDir(const std::string &path);
	inline bool IsPathExist(const char *path) { return access(path, 0) == 0; }
	inline bool IsDirExist(const char *dirPath) { return IsPathExist(dirPath); }
	inline bool IsDirExist(const std::string &dirPath) { return IsPathExist(dirPath.c_str()); }
	inline bool IsFileExist(const char *filePath) { return IsPathExist(filePath); }
	inline bool IsFileExist(const std::string &filePath) { return IsPathExist(filePath.c_str()); }
	inline bool MakeDir(const std::string &dirPath) { return MakeDir(dirPath.c_str()); }
	inline bool DeleteFile(const std::string &filePath) { return DeleteFile(filePath.c_str()); }
	inline bool DeleteFile(const char *filePath) { return remove(filePath) == 0; }
}

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#include <Windows.h>
#define popen _popen
#define pclose _pclose
#else
#include <cstring>
#include <dirent.h>
#define mkdir(dst) mkdir(dst, 0755)
#endif

namespace os
{
#ifdef _WIN32
	std::vector<std::string> GetDirectoryList(const char *path)
	{
		std::string dirPath(path);
		WIN32_FIND_DATAA findData;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		switch (dirPath[dirPath.size() - 1])
		{
		case '\\':
			dirPath += "*";
			break;
		case '/':
			dirPath += "*";
			break;
		default:
			dirPath += "\\*";
			break;
		}
		std::vector<std::string> dirList;
		hFind = FindFirstFileA(dirPath.c_str(), &findData);
		if (hFind == INVALID_HANDLE_VALUE)
			return dirList;
		while (FindNextFileA(hFind, &findData) != 0)
		{
			if (strcmp("..", findData.cFileName) == 0)
				continue;
			dirList.push_back(std::string(findData.cFileName));
		}

		FindClose(hFind);
		return dirList;
	}
#else
	std::vector<std::string> GetDirectoryList(const char *path)
	{
		std::vector<std::string> list;
		DIR *d;
		struct dirent *dir;
		d = opendir(path);
		if (d)
		{
			while ((dir = readdir(d)) != NULL)
			{
				if (strcmp(dir->d_name, "..") == 0)
					continue;
				if (strcmp(dir->d_name, ".") == 0)
					continue;
				list.push_back(dir->d_name);
			}
			closedir(d);
		}
		return list;
	}
#endif

	ExecuteResult Execute(const std::string &s)
	{
		std::string cmd = s;
		cmd += " 2>&1";
		FILE *file = popen(s.c_str(), "r");
		if (file == NULL)
		{
			cmd = "command not found";
			return ExecuteResult{-1, std::move(cmd)};
		}
		const int bufferSize = 256;
		char buffer[bufferSize];
		cmd = "";
		while (fgets(buffer, bufferSize, file) != NULL)
		{
			cmd += buffer;
		}
		return ExecuteResult{pclose(file), std::move(cmd)};
	}

	bool MakeDir(const char *dirPath)
	{
		char *buffer = (char *)malloc(strlen(dirPath) + 1);
		strcpy(buffer, dirPath);
		int index = 0;
		while (*dirPath++ != '\0')
		{
			if (*dirPath == '/' || *dirPath == '\\')
			{
				buffer[index + 1] = '\0';
				if (!IsDirExist(buffer))
					if (mkdir(buffer) != 0)
						return false;
				buffer[index + 1] = '/';
			}
			index++;
		}
		index = 0;
		if (!IsDirExist(buffer))
			index = mkdir(buffer);
		free(buffer);
		return index == 0;
	}

	ExecuteResult Execute(const char *cmd) { return Execute(std::string(cmd)); }
	std::vector<std::string> ListDir(const char *path) { return GetDirectoryList(path); }
	std::vector<std::string> ListDir(const std::string &path) { return GetDirectoryList(path.c_str()); }
}

#endif