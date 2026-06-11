#include "DB.h"

std::unique_ptr<DBConnection> CreateDBConnection(sql::mysql::MySQL_Driver* pDriver)
{
    sql::Connection* pConnection = pDriver->connect("tcp://192.168.55.162:3306", "sa_server", ")5T_YvXk5D1@");    // test (추후에는 파일 입력으로 대체)

    std::unique_ptr<sql::Connection> upConnection(pConnection);
    upConnection->setSchema("sa_db");

    std::unique_ptr<DBConnection> upDBConn = std::make_unique<DBConnection>(std::move(upConnection));

    return upDBConn;
}

DBConnection::DBConnection(std::unique_ptr<sql::Connection> upConnection)
    : m_upConnection(std::move(upConnection))
{
    // Prepared Statement 생성
    
    // INSERT USER
    m_stmts[static_cast<size_t>(PreparedStatementId::InsertUser)] =
        std::unique_ptr<sql::PreparedStatement>(m_upConnection->prepareStatement(UserSQL::INSERT_USER));

    // ID DUPLICATE CHECK
    m_stmts[static_cast<size_t>(PreparedStatementId::IdDuplicateCheck)] =
        std::unique_ptr<sql::PreparedStatement>(m_upConnection->prepareStatement(UserSQL::ID_DUPLICATE_CHECK));

    // NICKNAME DUPLICATE CHECK
    m_stmts[static_cast<size_t>(PreparedStatementId::NicknameDuplicateCheck)] =
        std::unique_ptr<sql::PreparedStatement>(m_upConnection->prepareStatement(UserSQL::NICKNAME_DUPLICATE_CHECK));
}

DBConnectionPool::DBConnectionPool()
    : m_pDriver(sql::mysql::get_mysql_driver_instance())
{
    InitializeSRWLock(&m_lock);
}

std::unique_ptr<DBConnection> DBConnectionPool::GetConnection()
{
    AcquireSRWLockExclusive(&m_lock);

    if (m_connections.empty())
    {
        ReleaseSRWLockExclusive(&m_lock);

        return CreateDBConnection(m_pDriver);
    }
    else
    {
        std::unique_ptr<DBConnection> upDBConn = std::move(m_connections.top());
        m_connections.pop();

        ReleaseSRWLockExclusive(&m_lock);

        return upDBConn;
    }
}

void DBConnectionPool::ReturnConnection(std::unique_ptr<DBConnection> upDBConn)
{
    AcquireSRWLockExclusive(&m_lock);
    m_connections.push(std::move(upDBConn));
    ReleaseSRWLockExclusive(&m_lock);
}
