#include "DB.h"

class SQLQuery
{
public:

    // =========================
    // INSERT
    // =========================
    static constexpr const char* INSERT_USER =
        "INSERT INTO users (`login_id`, `nickname`, `login_hash_pw`) VALUES (?, ?, ?);";

    // =========================
    // LOGIN / AUTH
    // =========================
    static constexpr const char* SELECT_USER_BY_LOGIN =
        "SELECT `account_id`, `nickname`, `level`, `exp`, `point` FROM `users` WHERE `login_id` = ? AND `login_hash_pw` = ? LIMIT 1;";

    // =========================
    // EXISTS CHECK
    // =========================
    static constexpr const char* EXIST_LOGIN_ID =
        "SELECT 1 FROM `users` WHERE `login_id` = ? LIMIT 1";

    static constexpr const char* EXIST_NICKNAME =
        "SELECT 1 FROM `users` WHERE `nickname` = ? LIMIT 1";
};

void PrintSQLExceptionLog(const sql::SQLException& e)
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

        this->CreatePreparedStatements();
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

    // INSERT_USER
    m_pstmts[static_cast<size_t>(PreparedStatementId::InsertUser)] = 
        std::unique_ptr<sql::PreparedStatement>(m_upConnection->prepareStatement(SQLQuery::INSERT_USER));

    // LOGIN_ID
    m_pstmts[static_cast<size_t>(PreparedStatementId::SelectUserByLogin)] =
        std::unique_ptr<sql::PreparedStatement>(m_upConnection->prepareStatement(SQLQuery::SELECT_USER_BY_LOGIN));

    // EXISTS_LOGIN_ID
    m_pstmts[static_cast<size_t>(PreparedStatementId::ExistLoginId)] =
        std::unique_ptr<sql::PreparedStatement>(m_upConnection->prepareStatement(SQLQuery::EXIST_LOGIN_ID));

    // EXISTS_NICKNAME
    m_pstmts[static_cast<size_t>(PreparedStatementId::ExistNickname)] =
        std::unique_ptr<sql::PreparedStatement>(m_upConnection->prepareStatement(SQLQuery::EXIST_NICKNAME));
}
