#pragma once
#ifndef __DB_UTIL_H
#define __DB_UTIL_H

#include <mysql.h>
#include <string>
#include <vector>
#pragma comment(lib, "libmysql.lib")

#ifndef ExportDLL
#define ExportDLL __declspec(dllexport)
#endif

namespace db
{
	using MySQLRow = std::vector<std::string>;
	using ResultVector = std::vector<MySQLRow>;
	using MySQLResult = struct
	{
		int errCode;
		std::string errMsg;
		ResultVector vector;
	};

	using MySQLExecRes = struct
	{
		int errCode;
		std::string errMsg;
		my_ulonglong affectedRows;
	};

	class MySQL
	{
	public:
		ExportDLL MySQL();
		ExportDLL ~MySQL();

		ExportDLL bool Connect(const char *host, int port,
							   const char *user,
							   const char *password,
							   const char *database);
		ExportDLL const char *Error();
		ExportDLL unsigned int Errno();
		ExportDLL MySQLResult Query(const char *command);
		ExportDLL MySQLExecRes Execute(const char *command);

	private:
		MYSQL mysql;
	};
}

#endif
