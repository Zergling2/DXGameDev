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

class DBJobIdDuplicateCheck : public IDBJob
{
public:
	DBJobIdDuplicateCheck(uint64_t netId, const char* id);
	virtual ~DBJobIdDuplicateCheck() = default;

	virtual bool IsValid() const override { return m_isValid; }
	virtual void Execute(DBThread& thread) override;
private:
	bool m_isValid;
	uint64_t m_netId;
	char m_id[MAX_ID_LEN + 1];
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
private:
	SAServer& m_server;
	DBConnection m_dbConn;
};
