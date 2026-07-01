#include "DB.h"

class SQLQuery
{
public:
    static constexpr const char* INSERT_USER =
        "INSERT INTO users (`login_id`, `nickname`, `login_hash_pw`) VALUES (?, ?, ?);";

    static constexpr const char* ID_DUPLICATE_CHECK =
        "SELECT 1 FROM `users` WHERE `account_id`=? LIMIT 1";

    static constexpr const char* NICKNAME_DUPLICATE_CHECK =
        "SELECT 1 FROM `users` WHERE `nickname`=? LIMIT 1";
};

static void PrintSQLExceptionLog(const sql::SQLException& e)
{
    // 肺弊
    printf("Message: %s\nError Code: %d\nSQL State: %s\n", e.what(), e.getErrorCode(), e.getSQLState().c_str());
}

DBConnection::DBConnection(sql::mysql::MySQL_Driver* pDriver)
    : m_pDriver(pDriver)
    , m_upConnection()
    , m_pstmts(static_cast<size_t>(PreparedStatementId::Count))
{
}

bool DBConnection::Connect(const char* hostName, const char* userName, const char* password, const char* schema)
{
    try
    {
        std::unique_ptr<sql::Connection> upConnection(m_pDriver->connect(hostName, userName, password));

        upConnection->setSchema(schema);

        m_upConnection = std::move(upConnection);
    }
    catch (const sql::SQLException& e)
    {
        printf("Exception while calling sql::mysql::MySQL_Driver::connect()\n");
        PrintSQLExceptionLog(e);
        return false;
    }

    return true;
}

void DBConnection::Release()
{
    // PreparesStatements 秦力
    m_pstmts.clear();

    // 目池记 秦力
    m_upConnection.reset();
}

void DBConnection::CreatePreparedStatements()
{
    // Prepared Statement 积己

    // INSERT USER
    m_pstmts[static_cast<size_t>(PreparedStatementId::InsertUser)] = 
        std::unique_ptr<sql::PreparedStatement>(m_upConnection->prepareStatement(SQLQuery::INSERT_USER));

    // ID DUPLICATE CHECK
    m_pstmts[static_cast<size_t>(PreparedStatementId::IdDuplicateCheck)] =
        std::unique_ptr<sql::PreparedStatement>(m_upConnection->prepareStatement(SQLQuery::ID_DUPLICATE_CHECK));

    // NICKNAME DUPLICATE CHECK
    m_pstmts[static_cast<size_t>(PreparedStatementId::NicknameDuplicateCheck)] =
        std::unique_ptr<sql::PreparedStatement>(m_upConnection->prepareStatement(SQLQuery::NICKNAME_DUPLICATE_CHECK));
}
