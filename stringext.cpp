#include "stringext.h"

namespace stringext
{
	void ReplaceAll(std::string &s, const std::string &from, const std::string &to)
	{
		size_t index = 0;
		while ((index = s.find(from, index)) != std::string::npos)
		{
			s.replace(index, from.length(), to);
			index += to.length();
		}
	}

	void ReplaceAll(std::string &s, const char *from, const char *to) { ReplaceAll(s, std::string(from), std::string(to)); }
	std::vector<std::string> Split(const std::string &s, const std::string &sep)
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
			v.push_back(s.substr(begin, s.length() - begin));
		return v;
	}

	std::vector<std::string> Split(const std::string &s, const char *sep) { return Split(s, std::string(sep)); }
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
				size++;
			else
				break;
		}
		s.replace(0, size, "");
	}

	void TrimSpaceRight(std::string &s)
	{
		size_t size = 0;
		for (auto it = s.rbegin(); it < s.rend(); it++)
		{
			if (SpaceCharacters.find(*it) != std::string::npos)
				size++;
			else
				break;
		}
		s.replace(s.length() - size, size, "");
	}

	std::string Join(const std::vector<std::string> &stringVector, const std::string &sep)
	{
		if (stringVector.size() == 0)
			return "";
		std::string result;
		for (std::vector<std::string>::const_iterator it = stringVector.begin(); it < stringVector.end() - 1; it++)
			result += *it + sep;
		result += *(stringVector.end() - 1);
		return result;
	}

	std::string Join(const std::vector<std::string> &stringVector, const char *sep)
	{
		if (stringVector.size() == 0)
			return "";
		std::string result;
		for (std::vector<std::string>::const_iterator it = stringVector.begin(); it < stringVector.end() - 1; it++)
			result += *it + sep;
		result += *(stringVector.end() - 1);
		return result;
	}

	void ToUpper(std::string &s)
	{
		for (std::string::iterator it = s.begin(); it < s.end(); it++)
			*it = ::toupper(*it);
	}

	void ToLower(std::string &s)
	{
		for (std::string::iterator it = s.begin(); it < s.end(); it++)
			*it = ::tolower(*it);
	}
}
