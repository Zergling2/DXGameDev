#pragma once

#include "JobThreadBase.h"
#include <cstdint>
#include "DB.h"
#include "Constants.h"

class SAServer;
class DBThread;

class IDBJob
{
public:
	IDBJob() = default;
	virtual ~IDBJob() = default;

	virtual bool IsValid() const = 0;
	virtual void Execute(DBThread& thread) = 0;
};

class DBJobBase : public IDBJob
{
public:
	DBJobBase()
		: m_isValid(false)
	{
	}
	virtual ~DBJobBase() = default;

	virtual bool IsValid() const override { return m_isValid; }
protected:
	bool m_isValid;
};

class DBJobLogin : public DBJobBase
{
public:
	DBJobLogin(uint64_t netId, const wchar_t* id, const wchar_t* pw) noexcept;
	virtual ~DBJobLogin() = default;

	virtual void Execute(DBThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_id[MAX_ID_LEN + 1];
	wchar_t m_pw[MAX_PW_LEN + 1];
};

class DBJobCreateAccount : public DBJobBase
{
public:
	DBJobCreateAccount(uint64_t netId, const wchar_t* id, const wchar_t* nickname, const wchar_t* pw) noexcept;
	virtual ~DBJobCreateAccount() = default;

	virtual void Execute(DBThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_id[MAX_ID_LEN + 1];
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];
	wchar_t m_pw[MAX_PW_LEN + 1];
};

class DBJobIdDuplicateCheck : public DBJobBase
{
public:
	DBJobIdDuplicateCheck(uint64_t netId, const wchar_t* id) noexcept;
	virtual ~DBJobIdDuplicateCheck() = default;

	virtual void Execute(DBThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_id[MAX_ID_LEN + 1];
};

class DBJobNicknameDuplicateCheck : public DBJobBase
{
public:
	DBJobNicknameDuplicateCheck(uint64_t netId, const wchar_t* nickname) noexcept;
	virtual ~DBJobNicknameDuplicateCheck() = default;

	virtual void Execute(DBThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];
};

class DBThread : public JobThreadBase<IDBJob>
{
public:
	DBThread(SAServer& server);
	virtual ~DBThread() = default;
protected:
	virtual void OnThreadStart() override;
	virtual void OnThreadExit() override;
	virtual void ProcessJob(std::unique_ptr<IDBJob> upJob) override;
public:
	SAServer& m_server;
	DBConnection m_dbConnection;
};
