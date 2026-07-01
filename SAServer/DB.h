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
	IdDuplicateCheck,
	NicknameDuplicateCheck,

	Count
};

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

class UserDBService
{
public:
	static bool RegisterUser(sql::Connection* pConn, const std::string& id, const std::string& nickname, const std::string& pw);
	static bool Login(sql::Connection* pConn, const std::string& id, const std::string& pw);
	static bool AddPoint(sql::Connection* pConn, uint32_t userId, int32_t point);
	static bool ConsumePoint(sql::Connection* pConn, uint32_t userId, int32_t point);
	static bool LevelUp(sql::Connection* pConn, uint32_t userId);
};

class UserRepository
{
public:
	static bool IsDuplicateId(sql::Connection* pConn, const std::string& id);
	static bool IsDuplicateNickname(sql::Connection* pConn, const std::string& nickname);
	static void InsertUser(sql::Connection* pConn, const std::string& id, const std::string& nickname, const std::string& pw);
};
