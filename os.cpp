#include "os.h"

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
	ExecuteResult Execute(const char *cmd) { return Execute(std::string(cmd)); }
	bool IsPathExist(const char *path) { return access(path, 0) == 0; }
	bool IsDirExist(const char *dirPath) { return IsPathExist(dirPath); }
	bool IsDirExist(std::string &dirPath) { return IsPathExist(dirPath.c_str()); }
	bool IsFileExist(const char *filePath) { return IsPathExist(filePath); }
	bool IsFileExist(std::string &filePath) { return IsPathExist(filePath.c_str()); }

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
		return index == 0;
	}

	bool MakeDir(std::string &dirPath)
	{
		return MakeDir(dirPath.c_str());
	}

}