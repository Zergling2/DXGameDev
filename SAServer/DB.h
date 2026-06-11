#pragma once

#include <jdbc/mysql_connection.h>
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_error.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <memory>
#include <stack>
#include <array>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class UserSQL
{
public:
	static constexpr const char* INSERT_USER =
		"INSERT INTO users (`login_id`, `nickname`, `login_hash_pw`) VALUES (?, ?, ?);";

	static constexpr const char* ID_DUPLICATE_CHECK =
		"SELECT 1 FROM `users` WHERE `account_id`=? LIMIT 1";

	static constexpr const char* NICKNAME_DUPLICATE_CHECK =
		"SELECT 1 FROM `users` WHERE `nickname`=? LIMIT 1";
};

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
	DBConnection(std::unique_ptr<sql::Connection> upConnection);

	sql::Connection* Get() const { return m_upConnection.get(); }
	sql::PreparedStatement* GetStatement(PreparedStatementId stmtId) const { return m_stmts[static_cast<size_t>(stmtId)].get(); }
private:
	std::unique_ptr<sql::Connection> m_upConnection;
	std::array<std::unique_ptr<sql::PreparedStatement>, static_cast<size_t>(PreparedStatementId::Count)> m_stmts;
};

class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool() = default;

	std::unique_ptr<DBConnection> GetConnection();
	void ReturnConnection(std::unique_ptr<DBConnection> upDBConn);
private:
	sql::mysql::MySQL_Driver* m_pDriver;
	std::stack<std::unique_ptr<DBConnection>> m_connections;
	SRWLOCK m_lock;
};

// DB Connection RAII °´Ã¼
class DBConnectionHandle
{
public:
	DBConnectionHandle(DBConnectionPool* pConnectionPool, std::unique_ptr<DBConnection> upDBConn)
		: m_pConnectionPool(pConnectionPool)
		, m_upDBConn(std::move(upDBConn))
	{
	}
	~DBConnectionHandle()
	{
		if (m_upDBConn)
			m_pConnectionPool->ReturnConnection(std::move(m_upDBConn));
	}
	DBConnection* operator->() const { return m_upDBConn.get(); }
	DBConnection& operator*() const { return *m_upDBConn; }
	DBConnection* Get() const { return m_upDBConn.get(); }
private:
	DBConnectionPool* m_pConnectionPool;
	std::unique_ptr<DBConnection> m_upDBConn;
};

class UserRepository
{
public:
	UserRepository(DBConnection& dbConn)
		: m_dbConn(dbConn)
	{
	}
	~UserRepository() = default;

	bool IsDuplicateId(const std::string& id);
	bool IsDuplicateNickname(const std::string& nickname);
	void InsertUser(const std::string& id, const std::string& nickname, const std::string& pw);
private:
	DBConnection& m_dbConn;
};

class UserDBService
{
public:
	UserDBService(DBConnectionPool* pConnPool)
		: m_pConnPool(pConnPool)
	{
	}
	bool RegisterUser(const std::string& id, const std::string& nickname, const std::string& pw);
	bool Login(const std::string& id, const std::string& pw);
	bool AddPoint(uint32_t userId, int32_t point);
	bool ConsumePoint(uint32_t userId, int32_t point);
	bool LevelUp(uint32_t userId);
private:
	DBConnectionPool* m_pConnPool;
};
