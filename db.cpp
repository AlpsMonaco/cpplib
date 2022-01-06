#include "db.h"

namespace db
{
	MySQL::MySQL()
	{
		this->mysql = mysql_init(NULL);
	}

	MySQL::~MySQL()
	{
		mysql_close(this->mysql);
		this->mysql = nullptr;
		mysql_thread_end();
	}

	MySQLResult MySQL::Query(const char *command)
	{
		MySQLResult result{0};
		if (mysql_query(this->mysql, command))
		{
			result.errCode = this->Errno();
			result.errMsg = std::string(this->Error());
			return result;
		}
		MYSQL_RES *res = mysql_store_result(this->mysql);
		unsigned int numFields = mysql_num_fields(res);
		result.vector.reserve(mysql_num_rows(res));

		MYSQL_ROW row;
		while ((row = mysql_fetch_row(res)) != NULL)
		{
			MySQLRow r;
			for (unsigned int i = 0; i < numFields; i++)
			{
				r.push_back(std::string(row[i]));
			}
			result.vector.push_back(std::move(r));
		}
		mysql_free_result(res);
		return result;
	}

	MySQLExecRes MySQL::Execute(const char *command)
	{
		MySQLExecRes result;
		result.errCode = 0;
		result.affectedRows = 0;
		if (mysql_query(this->mysql, command))
		{
			result.errCode = this->Errno();
			result.errMsg = std::string(this->Error());
			return result;
		}
		result.affectedRows = mysql_affected_rows(this->mysql);
		return result;
	}
}
