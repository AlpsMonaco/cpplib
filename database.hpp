#pragma once
#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include "thirdparty/include/mysql/mysql.h"
#include <string>
#include <vector>
#include <utility>

#ifdef _WIN32
#pragma comment(lib, "libmysql.lib")
#endif

namespace database
{
	struct Result
	{
		Result() : code(0), msg() {}
		Result(int code, std::string &msg) : code(code), msg(msg) {}
		Result(int code, std::string &&msg) : code(code), msg(std::forward<std::string>(msg)) {}
		int code;
		std::string msg;
	};

	using Row = std::vector<std::string>;
	using Rows = std::vector<Row>;

	class MySQL
	{
	public:
		inline MySQL();
		~MySQL();

		inline bool Connect(const char *host, const int &port, const char *user, const char *password, const char *database);
		inline const char *Error();
		inline unsigned int Errno();
		inline Result Execute(const char *command);
		inline unsigned int GetAffectedRows();

		Rows GetRows();
		Row GetColumns();
		inline void Close();

	protected:
		inline void Free();

	private:
		MYSQL *mysql;
		MYSQL_RES *mysqlres;
	};
}

namespace database
{
	bool MySQL::Connect(const char *host, const int &port, const char *user, const char *password, const char *database) { return mysql_real_connect(this->mysql, host, user, password, database, port, nullptr, 0) != NULL; }
	const char *MySQL::Error() { return mysql_error(this->mysql); }
	unsigned int MySQL::Errno() { return mysql_errno(this->mysql); }

	MySQL::MySQL() : mysql(nullptr), mysqlres(nullptr) { this->mysql = mysql_init(NULL); }
	MySQL::~MySQL() { this->Free(); }
	void MySQL::Close() { this->Free(); }

	void MySQL::Free()
	{
		if (this->mysqlres != nullptr)
		{
			mysql_free_result(this->mysqlres);
			this->mysqlres = nullptr;
		}

		if (this->mysql != nullptr)
		{
			mysql_close(this->mysql);
			this->mysql = nullptr;
			mysql_thread_end();
		}
	}

	Row MySQL::GetColumns()
	{
		Row row;
		if (this->mysqlres != nullptr)
		{
			MYSQL_FIELD *pfield;
			while ((pfield = mysql_fetch_field(this->mysqlres)) != nullptr)
				row.emplace_back(pfield->name);
		}
		return row;
	}

	Result MySQL::Execute(const char *command)
	{
		if (mysql_query(this->mysql, command))
			return Result(this->Errno(), std::string(this->Error()));
		if (this->mysqlres != nullptr)
			mysql_free_result(this->mysqlres);
		this->mysqlres = mysql_store_result(this->mysql);
		return Result();
	}

	Rows MySQL::GetRows()
	{
		Rows rows;
		if (this->mysqlres != nullptr)
		{
			unsigned int numfield = mysql_num_fields(this->mysqlres);
			MYSQL_ROW mysqlrow;
			while ((mysqlrow = mysql_fetch_row(this->mysqlres)) != NULL)
			{
				Row row;
				for (unsigned int i = 0; i < numfield; i++)
					row.emplace_back(std::string(mysqlrow[i] ? mysqlrow[i] : ""));
				rows.emplace_back(std::move(row));
			}
		}
		return rows;
	}

	unsigned int MySQL::GetAffectedRows() { return mysql_affected_rows(mysql); }
}

#endif
