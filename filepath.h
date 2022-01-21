#pragma once

#ifndef __FILEPATH_H
#define __FILEPATH_H
#include <string>

namespace filepath
{
	void ToSlash(std::string &filepath);
	void ToBackslash(std::string &filepath);
	std::string ExtName(const std::string &filepath);
	std::string BaseName(const std::string &filepath);
	std::string LastElement(const std::string &filepath);
}

#endif