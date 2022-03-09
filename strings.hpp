#ifndef __STRINGS_HPP__
#define __STRINGS_HPP__

#include <string>
#include <vector>
#include <fstream>

namespace strings
{
	template <typename T>
	std::string ReadFileAll(T &fs) { return std::string(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>()); }

	template <typename T>
	std::string Join(const std::vector<std::string> &stringlist, const T &t)
	{
		if (stringlist.size() == 0)
			return "";
		std::string result;
		for (std::vector<std::string>::const_iterator it = stringlist.begin(); it < stringlist.end() - 1; it++)
			result += *it + t;
		result += *(stringlist.end() - 1);
		return result;
	}

	void ReplaceAll(std::string &s, const std::string &from, const std::string &to);
	std::vector<std::string> Split(const std::string &s, const std::string &sep);
	void TrimSpaceLeft(std::string &s);
	void TrimSpaceRight(std::string &s);
	void TrimSpace(std::string &s);
	void ToUpper(std::string &s);
	void ToLower(std::string &s);
}

namespace strings
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

	void TrimSpaceLeft(std::string &s)
	{
		static const std::string space = " \t\r\n";
		size_t size = 0;
		for (auto it = s.begin(); it < s.end(); it++)
		{
			if (space.find(*it) != std::string::npos)
				size++;
			else
				break;
		}
		s.replace(0, size, "");
	}

	void TrimSpaceRight(std::string &s)
	{
		static const std::string space = " \t\r\n";
		size_t size = 0;
		for (auto it = s.rbegin(); it < s.rend(); it++)
		{
			if (space.find(*it) != std::string::npos)
				size++;
			else
				break;
		}
		s.replace(s.length() - size, size, "");
	}

	void TrimSpace(std::string &s)
	{
		TrimSpaceLeft(s);
		TrimSpaceRight(s);
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

#endif