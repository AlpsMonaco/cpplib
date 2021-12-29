#include "strings.h"

namespace strings
{
	void ReplaceString(std::string &s, std::string &from, std::string &to)
	{
		size_t index = 0;
		while ((index = s.find(from, index)) != std::string::npos)
		{
			s.replace(index, from.length(), to);
			index += to.length();
		}
	}

	void ReplaceString(std::string &s, const char *from, const char *to)
	{
		ReplaceString(s, std::string(from), std::string(to));
	}

	std::vector<std::string> SplitString(const std::string &s, const std::string &sep)
	{
		size_t begin = 0;
		size_t end = 0;
		std::vector<std::string> v;

		while ((end = s.find(sep, begin)) != std::string::npos)
		{
			v.push_back(s.substr(begin, end - begin));
			begin = end + sep.length();
		}
		if (s.length() > begin)
		{
			v.push_back(s.substr(begin, s.length() - begin));
		}
		return v;
	}

	std::vector<std::string> SplitString(const std::string &s, const char *sep)
	{
		return SplitString(s, std::string(sep));
	}

	std::string ReadFileAll(const char *fileName)
	{
		std::ifstream ifs(fileName);
		if (!ifs.is_open())
			return "";
		return std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
	}

	const std::string SpaceCharacters = " \t\r\n";

	void TrimSpace(std::string &s)
	{
		TrimSpaceLeft(s);
		TrimSpaceRight(s);
	}

	void TrimSpaceLeft(std::string &s)
	{
		size_t size = 0;
		for (auto it = s.begin(); it < s.end(); it++)
		{
			if (SpaceCharacters.find(*it) != std::string::npos)
			{
				size++;
			}
			else
			{
				break;
			}
		}
		s.replace(0, size, "");
	}

	void TrimSpaceRight(std::string &s)
	{
		size_t size = 0;
		for (auto it = s.rbegin(); it < s.rend(); it++)
		{
			if (SpaceCharacters.find(*it) != std::string::npos)
			{
				size++;
			}
			else
			{
				break;
			}
		}
		s.replace(s.length() - size, size, "");
	}

	std::string Join(std::vector<std::string> &stringVector, const std::string &sep)
	{
		if (stringVector.size() == 0)
			return "";

		std::string result;
		for (std::vector<std::string>::iterator it = stringVector.begin(); it < stringVector.end() - 1; it++)
		{
			result += *it + sep;
		}
		result += *(stringVector.end() - 1);
		return result;
	}

	std::string Join(std::vector<std::string> &stringVector, const char *sep)
	{
		if (stringVector.size() == 0)
			return "";

		std::string result;
		for (std::vector<std::string>::iterator it = stringVector.begin(); it < stringVector.end() - 1; it++)
		{
			result += *it + sep;
		}
		result += *(stringVector.end() - 1);
		return result;
	}
}
