#pragma once
#ifndef __STRING_EXT_H
#define __STRING_EXT_H
#include <string>
#include <vector>
#include <fstream>

namespace stringext
{
	void ReplaceAll(std::string &s, const std::string &from, const std::string &to);
	void ReplaceAll(std::string &s, const char *from, const char *to);
	std::vector<std::string> Split(const std::string &s, const std::string &sep);
	std::vector<std::string> Split(const std::string &s, const char *sep);
	template <typename T>
	std::string ReadFileAll(T &fs) { return std::string(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>()); }
	void TrimSpaceLeft(std::string &s);
	void TrimSpaceRight(std::string &s);
	void TrimSpace(std::string &s);
	std::string Join(const std::vector<std::string> &stringVector, const std::string &sep);
	std::string Join(const std::vector<std::string> &stringVector, const char *sep);
	void ToUpper(std::string &s);
	void ToLower(std::string &s);
}

#endif