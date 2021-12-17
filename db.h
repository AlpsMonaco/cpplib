#pragma once
#ifndef __DB_UTIL_H
#define __DB_UTIL_H

#include <mysql.h>
#include <string>
#include <vector>
#pragma comment(lib, "libmysql.lib")

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
		MySQL();
		~MySQL();

		bool Connect(const char *host, int port,
					 const char *user,
					 const char *password,
					 const char *database);
		const char *Error();
		unsigned int Errno();
		MySQLResult Query(const char *command);
		MySQLExecRes Execute(const char *command);

	private:
		MYSQL mysql;
	};
}

#endif
