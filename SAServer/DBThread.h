#pragma once

#include "JobThreadBase.h"
#include <cstdint>
#include "Constants.h"

class DBThread;

class IDBJob
{
public:
	IDBJob() = default;
	virtual ~IDBJob() = default;

	virtual void Execute(DBThread& thread) = 0;
};

class DBJobIdDuplicateCheck : public IDBJob
{
public:
	DBJobIdDuplicateCheck(uint64_t netId, const char* id);
	virtual ~DBJobIdDuplicateCheck() = default;

	virtual void Execute(DBThread& thread) override;
private:
	uint64_t m_netId;
	char m_id[MAX_ID_LEN + 1];
};

class DBThread : public JobThreadBase<IDBJob>
{
public:
	DBThread() = default;
	virtual ~DBThread() = default;
protected:
	virtual void OnThreadStart() override;
	virtual void OnThreadExit() override;
	virtual void ProcessJob(std::unique_ptr<IDBJob> upJob) override;
};
