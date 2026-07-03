#pragma once

#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_error.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <memory>
#include <stack>
#include <vector>

enum class PreparedStatementId
{
	InsertUser,

	SelectUserByLogin,

	ExistLoginId,
	ExistNickname,

	Count
};

void PrintSQLExceptionLog(const sql::SQLException& e);

class DBConnection
{
public:
	DBConnection(sql::mysql::MySQL_Driver* pDriver);

	bool Connect(const char* hostName, const char* userName, const char* password, const char* schema);
	void Release();

	sql::Connection* GetConnection() const { return m_upConnection.get(); }
	sql::PreparedStatement* GetPreparedStatement(PreparedStatementId stmtId) const { return m_pstmts[static_cast<size_t>(stmtId)].get(); }
private:
	void CreatePreparedStatements();
private:
	sql::mysql::MySQL_Driver* m_pDriver;
	std::unique_ptr<sql::Connection> m_upConnection;
	std::vector<std::unique_ptr<sql::PreparedStatement>> m_pstmts;
};
