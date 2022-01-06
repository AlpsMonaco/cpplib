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

		inline bool MySQL::Connect(const char *host, int port, const char *user, const char *password, const char *database) { return mysql_real_connect(this->mysql, host, user, password, database, port, nullptr, 0) != NULL; }
		inline const char *MySQL::Error() { return mysql_error(this->mysql); }
		inline unsigned int MySQL::Errno() { return mysql_errno(this->mysql); }
		MySQLResult Query(const char *command);
		MySQLExecRes Execute(const char *command);

	private:
		MYSQL *mysql;
	};
}

#endif
