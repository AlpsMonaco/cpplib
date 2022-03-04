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
		int code;
		std::string output;
		friend std::ostream &operator<<(std::ostream &os, const ExecuteResult &result);
		ExecuteResult(int code, const std::string &result) : code(code), output(result) {}
		ExecuteResult(int code, const std::string &&result) : code(code), output(result) {}
	};

	ExecuteResult Execute(const std::string &cmd);
	std::vector<std::string> ListDir(const std::string &path);
	inline bool IsPathExist(const std::string &path);
	inline bool IsDirExist(const std::string &dirPath);
	inline bool IsFileExist(const std::string &filePath);
	inline bool MakeDir(const std::string &dirPath);
	inline bool RemoveFile(const std::string &filePath);
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
	std::vector<std::string> ListDir(const std::string &path)
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
	std::vector<std::string> ListDir(const std::string &path)
	{
		std::vector<std::string> list;
		DIR *d;
		struct dirent *dir;
		d = opendir(path.c_str());
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

	std::ostream &operator<<(std::ostream &os, const ExecuteResult &result)
	{
		os << result.code;
		if (result.output != "")
			os << std::endl
			   << result.output;
		return os;
	}

	ExecuteResult Execute(const std::string &s)
	{
		std::string cmd = s;
		cmd += " 2>&1";
		FILE *file = popen(s.c_str(), "r");
		if (file == NULL)
		{
			cmd = "command not found";
			return ExecuteResult(1, std::move(cmd));
		}
		const int bufferSize = 256;
		char buffer[bufferSize];
		cmd.clear();
		while (fgets(buffer, bufferSize, file) != NULL)
			cmd += buffer;
		return ExecuteResult(pclose(file), std::move(cmd));
	}

	bool MakeDir(const std::string &dirPath)
	{
		const char *cdirPath = dirPath.c_str();
		char *buffer = (char *)malloc(strlen(cdirPath) + 1);
		strcpy(buffer, cdirPath);
		int index = 0;
		while (*cdirPath++ != '\0')
		{
			if (*cdirPath == '/' || *cdirPath == '\\')
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

	bool IsPathExist(const std::string &path) { return access(path.c_str(), 0) == 0; }
	bool IsDirExist(const std::string &dirPath) { return IsPathExist(dirPath); }
	bool IsFileExist(const std::string &filePath) { return IsPathExist(filePath); }
	bool RemoveFile(const std::string &filePath) { return remove(filePath.c_str()) == 0; }
}

#endif