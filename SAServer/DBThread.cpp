#include "DBThread.h"
#include <cassert>
#include <strsafe.h>
#include <openssl\sha.h>
#include "LogicThread.h"
#include "SAServer.h"

static const char g_hexMap[] = "0123456789abcdef";

static bool UTF16ToUTF8(const wchar_t* src, char* buf, int cbBufSize)
{
	if (src == nullptr || buf == nullptr || cbBufSize <= 0)
		return false;

	const int ret = WideCharToMultiByte(
		CP_UTF8,
		0,
		src,
		-1,	// src는 NULL 종료 문자열만 허용
		buf,
		cbBufSize,
		nullptr,
		nullptr
	);

	return ret != 0;
}

static bool UTF8ToUTF16(const char* src, wchar_t* buf, int cchBufSize)
{
	if (src == nullptr || buf == nullptr || cchBufSize <= 0)
		return false;

	int ret = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		src,
		-1,	// src는 NULL 종료 문자열만 허용
		buf,
		cchBufSize);

	return ret != 0;
}

DBThread::DBThread(SAServer& server)
	: m_server(server)
	, m_dbConnection(sql::mysql::get_mysql_driver_instance())
{
}

void DBThread::OnThreadStart()
{
	bool result = m_dbConnection.Connect("tcp://127.0.0.1:3306", "sa_server", ")5T_YvXk5D1@", "sa_db");

	if (result)
	{
		printf("db connection successful!\n");
	}
	else
	{
		printf("db connection failed!\n");
	}
}

void DBThread::OnThreadExit()
{
	m_dbConnection.Release();
}

void DBThread::ProcessJob(std::unique_ptr<IDBJob> upJob)
{
	upJob->Execute(*this);
}

DBJobLogin::DBJobLogin(uint64_t netId, const wchar_t* id, const wchar_t* pw) noexcept
	: m_netId(netId)
{
	m_isValid = false;

	do
	{
		if (FAILED(StringCchCopyW(m_id, _countof(m_id), id)))
			break;

		if (FAILED(StringCchCopyW(m_pw, _countof(m_pw), pw)))
			break;

		m_isValid = true;
	} while (false);
}

void DBJobLogin::Execute(DBThread& thread)
{
	char utf8Id[UTF8_MAX_ID_SIZE + 1];
	char utf8Pw[UTF8_MAX_PW_SIZE + 1];

	if (!UTF16ToUTF8(m_id, utf8Id, sizeof(utf8Id)) || !UTF16ToUTF8(m_pw, utf8Pw, sizeof(utf8Pw)))
		*reinterpret_cast<int*>(0) = 0;

	const size_t pwLen = strlen(utf8Pw);
	assert(MIN_PW_LEN <= pwLen && pwLen <= MAX_PW_LEN);

	unsigned char hashedPw[SHA256_DIGEST_LENGTH];
	SHA256(reinterpret_cast<unsigned char*>(utf8Pw), pwLen, hashedPw);

	char hashedPwStr[SHA256_DIGEST_LENGTH * 2 + 1];
	for (size_t i = 0; i < _countof(hashedPw); ++i)
	{
		hashedPwStr[i * 2] = g_hexMap[(hashedPw[i] & 0xf0) >> 4];
		hashedPwStr[i * 2 + 1] = g_hexMap[hashedPw[i] & 0x0f];
	}
	hashedPwStr[_countof(hashedPwStr) - 1] = '\0';	// null 종료 문자열화

	// 함수					용도							반환값
	// executeQuery()		SELECT						ResultSet*
	// executeUpdate()		INSERT, UPDATE, DELETE		영향받은 Row 수(int)
	// execute()			어떤 SQL인지 모를 때 범용		결과가 ResultSet이면 true, 아니면 false

	// PreparedStatement 매개변수 바인딩 및 조회
	sql::PreparedStatement* const pStmt = thread.m_dbConnection.GetPreparedStatement(PreparedStatementId::SelectUserByLogin);
	bool querySuccess = true;
	try
	{
		pStmt->setString(1, utf8Id);
		pStmt->setString(2, hashedPwStr);

		std::unique_ptr<sql::ResultSet> upResultSet(pStmt->executeQuery());

		if (upResultSet->next())
		{
			// 계정 존재
			std::unique_ptr<JobDBJobReqLoginResult> upDBResultJob = std::make_unique<JobDBJobReqLoginResult>(m_netId, true, true);

			/*
			static constexpr const char* SELECT_USER_BY_LOGIN_ID =
				"SELECT `account_id`, `nickname`, `level`, `exp`, `point` FROM `users` WHERE `login_id` = ? AND `login_hash_pw` = ? LIMIT 1;";
				             1            2          3       4       5
			*/


			wchar_t nickname[MAX_NICKNAME_LEN + 1];
			if (!UTF8ToUTF16(upResultSet->getString(2).c_str(), nickname, _countof(nickname)))		// select 기준 "nickname" 2번째 컬럼
				*reinterpret_cast<int*>(0) = 0;

			upDBResultJob->m_accountId = upResultSet->getInt(1);	// "account_id"
			upDBResultJob->m_nicknameLen = static_cast<uint16_t>(wcslen(nickname));
			assert(upDBResultJob->m_nicknameLen <= MAX_NICKNAME_LEN);
			wcscpy_s(upDBResultJob->m_nickname, nickname);
			upDBResultJob->m_level = upResultSet->getInt(3);	// "level"
			upDBResultJob->m_exp = upResultSet->getInt(4);		// "exp"
			upDBResultJob->m_point = upResultSet->getInt(5);	// "point"

			thread.m_server.GetLogicThread().DispatchJob(std::move(upDBResultJob));
		}
		else
		{
			// 계정 정보 없음
			std::unique_ptr<JobDBJobReqLoginResult> upDBResultJob = std::make_unique<JobDBJobReqLoginResult>(m_netId, true, false);
			thread.m_server.GetLogicThread().DispatchJob(std::move(upDBResultJob));
		}
	}
	catch (const sql::SQLException& e)
	{
		querySuccess = false;

		printf("Exception while DBJobLogin\n");
		PrintSQLExceptionLog(e);

		// DB 조회 실패
		std::unique_ptr<JobDBJobReqLoginResult> upDBResultJob = std::make_unique<JobDBJobReqLoginResult>(m_netId, false, false);
		thread.m_server.GetLogicThread().DispatchJob(std::move(upDBResultJob));
	}
}

DBJobCreateAccount::DBJobCreateAccount(uint64_t netId, const wchar_t* id, const wchar_t* nickname, const wchar_t* pw) noexcept
	: m_netId(netId)
{
	m_isValid = false;

	do
	{
		if (FAILED(StringCchCopyW(m_id, _countof(m_id), id)))
			break;

		if (FAILED(StringCchCopyW(m_nickname, _countof(m_nickname), nickname)))
			break;

		if (FAILED(StringCchCopyW(m_pw, _countof(m_pw), pw)))
			break;

		m_isValid = true;
	} while (false);
}

void DBJobCreateAccount::Execute(DBThread& thread)
{
	char utf8Id[UTF8_MAX_ID_SIZE + 1];
	char utf8Nickname[UTF8_MAX_NICKNAME_SIZE + 1];
	char utf8Pw[UTF8_MAX_PW_SIZE + 1];

	if (!UTF16ToUTF8(m_id, utf8Id, sizeof(utf8Id)) || !UTF16ToUTF8(m_nickname, utf8Nickname, sizeof(utf8Nickname)) || !UTF16ToUTF8(m_pw, utf8Pw, sizeof(utf8Pw)))
		*reinterpret_cast<int*>(0) = 0;

	const size_t pwLen = strlen(utf8Pw);
	assert(MIN_PW_LEN <= pwLen && pwLen <= MAX_PW_LEN);

	unsigned char hashedPw[SHA256_DIGEST_LENGTH];
	SHA256(reinterpret_cast<unsigned char*>(utf8Pw), pwLen, hashedPw);

	char hashedPwStr[SHA256_DIGEST_LENGTH * 2 + 1];
	for (size_t i = 0; i < _countof(hashedPw); ++i)
	{
		hashedPwStr[i * 2] = g_hexMap[(hashedPw[i] & 0xf0) >> 4];
		hashedPwStr[i * 2 + 1] = g_hexMap[hashedPw[i] & 0x0f];
	}
	hashedPwStr[_countof(hashedPwStr) - 1] = '\0';	// null 종료 문자열화

	// 함수					용도							반환값
	// executeQuery()		SELECT						ResultSet*
	// executeUpdate()		INSERT, UPDATE, DELETE		영향받은 Row 수(int)
	// execute()			어떤 SQL인지 모를 때 범용		결과가 ResultSet이면 true, 아니면 false

	// PreparedStatement 매개변수 바인딩 및 조회
	sql::PreparedStatement* const pStmt = thread.m_dbConnection.GetPreparedStatement(PreparedStatementId::InsertUser);
	bool querySuccess = true;
	try
	{
		pStmt->setString(1, utf8Id);
		pStmt->setString(2, utf8Nickname);
		pStmt->setString(3, hashedPwStr);

		bool inserted = pStmt->executeUpdate() > 0;
	}
	catch (const sql::SQLException& e)
	{
		querySuccess = false;

		printf("Exception while DBJobCreateAccount\n");
		PrintSQLExceptionLog(e);
	}

	// 결과 조회 및 잡 객체 생성
	std::unique_ptr<JobDBJobCreateAccountResult> upDBResultJob = std::make_unique<JobDBJobCreateAccountResult>(m_netId, querySuccess);

	// 로직 스레드에게 잡 디스패칭
	thread.m_server.GetLogicThread().DispatchJob(std::move(upDBResultJob));
}

DBJobIdDuplicateCheck::DBJobIdDuplicateCheck(uint64_t netId, const wchar_t* id) noexcept
	: m_netId(netId)
{
	HRESULT hr = StringCchCopyW(m_id, _countof(m_id), id);

	m_isValid = SUCCEEDED(hr);
}

void DBJobIdDuplicateCheck::Execute(DBThread& thread)
{
	char utf8Id[UTF8_MAX_ID_SIZE + 1];

	if (!UTF16ToUTF8(m_id, utf8Id, sizeof(utf8Id)))
		*reinterpret_cast<int*>(0) = 0;

	// 함수					용도							반환값
	// executeQuery()		SELECT						ResultSet*
	// executeUpdate()		INSERT, UPDATE, DELETE		영향받은 Row 수(int)
	// execute()			어떤 SQL인지 모를 때 범용		결과가 ResultSet이면 true, 아니면 false

	// PreparedStatement 매개변수 바인딩 및 조회
	sql::PreparedStatement* const pStmt = thread.m_dbConnection.GetPreparedStatement(PreparedStatementId::ExistLoginId);
	bool querySuccess = true;
	bool duplicated = true;
	try
	{
		pStmt->setString(1, utf8Id);

		auto upResultSet(pStmt->executeQuery());

		duplicated = upResultSet->next();
	}
	catch (const sql::SQLException& e)
	{
		querySuccess = false;

		printf("Exception while DBJobIdDuplicateCheck\n");
		PrintSQLExceptionLog(e);
	}

	// 결과 조회 및 잡 객체 생성
	std::unique_ptr<JobDBJobIdDuplicateCheckResult> upDBResultJob = 
		std::make_unique<JobDBJobIdDuplicateCheckResult>(m_netId, querySuccess, querySuccess ? duplicated : false);

	// 로직 스레드에게 잡 디스패칭
	thread.m_server.GetLogicThread().DispatchJob(std::move(upDBResultJob));
}

DBJobNicknameDuplicateCheck::DBJobNicknameDuplicateCheck(uint64_t netId, const wchar_t* nickname) noexcept
	: m_netId(netId)
{
	m_isValid = false;

	do
	{
		if (FAILED(StringCchCopyW(m_nickname, _countof(m_nickname), nickname)))
			break;

		m_isValid = true;
	} while (false);
}

void DBJobNicknameDuplicateCheck::Execute(DBThread& thread)
{
	char utf8Nickname[UTF8_MAX_NICKNAME_SIZE + 1];

	if (!UTF16ToUTF8(m_nickname, utf8Nickname, sizeof(utf8Nickname)))
		*reinterpret_cast<int*>(0) = 0;

	// 함수					용도							반환값
	// executeQuery()		SELECT						ResultSet*
	// executeUpdate()		INSERT, UPDATE, DELETE		영향받은 Row 수(int)
	// execute()			어떤 SQL인지 모를 때 범용		결과가 ResultSet이면 true, 아니면 false

	// PreparedStatement 매개변수 바인딩 및 조회
	sql::PreparedStatement* const pStmt = thread.m_dbConnection.GetPreparedStatement(PreparedStatementId::ExistNickname);
	bool querySuccess = true;
	bool duplicated = true;
	try
	{
		pStmt->setString(1, utf8Nickname);

		auto upResultSet(pStmt->executeQuery());

		duplicated = upResultSet->next();
	}
	catch (const sql::SQLException& e)
	{
		querySuccess = false;

		printf("Exception while DBJobNicknameDuplicateCheck\n");
		PrintSQLExceptionLog(e);
	}

	// 결과 조회 및 잡 객체 생성
	std::unique_ptr<JobDBJobNicknameDuplicateCheckResult> upDBResultJob =
		std::make_unique<JobDBJobNicknameDuplicateCheckResult>(m_netId, querySuccess, querySuccess ? duplicated : false);

	// 로직 스레드에게 잡 디스패칭
	thread.m_server.GetLogicThread().DispatchJob(std::move(upDBResultJob));
}
