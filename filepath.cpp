#include "filepath.h"

namespace filepath
{
	void ToSlash(std::string &filepath)
	{
		char *s = &filepath[0];
		char *c;
		while (*(c = (s++)) != '\0')
		{
			if (*c == '\\')
				*c = '/';
		}
	}

	void ToBackslash(std::string &filepath)
	{
		char *s = &filepath[0];
		char *c;
		while (*(c = (s++)) != '\0')
		{
			if (*c == '/')
				*c = '\\';
		}
	}

	std::string ExtName(const std::string &filepath)
	{
		size_t index = filepath.find_last_of(".");
		if (filepath.find_last_of('.') == std::string::npos)
			index = filepath.length();
		return filepath.substr(index, filepath.length() - index);
	}

	std::string LastElement(const std::string &filepath)
	{
		std::string path = filepath;
		ToSlash(path);
		size_t index = path.find_last_of("/");
		if (index == std::string::npos)
			index = -1;
		return path.substr(index + 1, path.length() - index - 1);
	}

	std::string BaseName(const std::string &filepath)
	{
		std::string el = LastElement(filepath);
		int i;
		for (i = el.size() - 1; i >= 0; i--)
		{
			if (el[i] == '.')
				break;
		}
		if (i == -1)
			i = el.length();
		return el.substr(0, i);
	}
}
