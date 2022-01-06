#include "os.h"

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
}