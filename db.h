#pragma once
#ifndef __DB_UTIL_H
#define __DB_UTIL_H

#include <mysql.h>
#include <string>
#include <vector>

#ifdef _WIN32
#pragma comment(lib, "libmysql.lib")
#endif

namespace db
{
	using MySQLRow = std::vector<std::string>;
	using ResultVector = std::vector<MySQLRow>;
	struct MySQLResult
	{
		int errCode;
		std::string errMsg;
		ResultVector vector;
	};
	struct MySQLExecRes
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

		inline bool Connect(const char *host, const int &port, const char *user, const char *password, const char *database) { return mysql_real_connect(this->mysql, host, user, password, database, port, nullptr, 0) != NULL; }
		inline const char *Error() { return mysql_error(this->mysql); }
		inline unsigned int Errno() { return mysql_errno(this->mysql); }
		MySQLResult Query(const char *command);
		MySQLExecRes Execute(const char *command);

	private:
		MYSQL *mysql;
	};
}

#endif
