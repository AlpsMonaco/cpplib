#pragma once
#ifndef __STRING_UTIL_H
#define __STRING_UTIL_H
#include <string>
#include <vector>
#include <fstream>

namespace stringutil
{
	void ReplaceString(std::string &s, std::string &from, std::string &to);
	void ReplaceString(std::string &s, const char *from, const char *to);
	std::vector<std::string> SplitString(const std::string &s, const std::string &sep);
	std::vector<std::string> SplitString(const std::string &s, const char *sep);
	std::string ReadFileAll(const char *fileName);
	template <typename T>
	std::string ReadFileAll(T &fs) { return std::string(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>()); }
	void TrimSpaceLeft(std::string &s);
	void TrimSpaceRight(std::string &s);
	void TrimSpace(std::string &s);
	std::string Join(std::vector<std::string> &stringVector, const std::string &sep);
	std::string Join(std::vector<std::string> &stringVector, const char *sep);
}

#endif