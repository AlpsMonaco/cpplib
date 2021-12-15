#pragma once
#ifndef __STRINGS_H
#define __STRINGS_H
#include <string>
#include <vector>
#include <fstream>

#ifndef ExportDLL
#define ExportDLL __declspec(dllexport)
#endif

namespace strings
{
	ExportDLL void ReplaceString(std::string &s, std::string &from, std::string &to);
	ExportDLL void ReplaceString(std::string &s, const char *from, const char *to);
	ExportDLL std::vector<std::string> SplitString(std::string &s, std::string &sep);
	ExportDLL std::vector<std::string> SplitString(std::string &s, const char *sep);
	ExportDLL std::string ReadFileAll(const char *fileName);
	template <typename T>
	ExportDLL std::string ReadFileAll(T &fs) { return std::string(std::istreambuf_iterator<char>(fs), std::istreambuf_iterator<char>()); }
	ExportDLL void TrimSpaceLeft(std::string &s);
	ExportDLL void TrimSpaceRight(std::string &s);
	ExportDLL void TrimSpace(std::string &s);
	ExportDLL std::string Join(std::vector<std::string> &stringVector, const std::string &sep);
	ExportDLL std::string Join(std::vector<std::string> &stringVector, const char *sep);
}

#endif