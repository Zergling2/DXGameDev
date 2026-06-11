#pragma once

#include "JobThreadBase.h"

class IDBJob
{
public:
	IDBJob() = default;
	virtual ~IDBJob() = default;

	virtual void Execute() = 0;
};

class DBThread : public JobThreadBase<IDBJob>
{
public:
	DBThread() = default;
	virtual ~DBThread() = default;
protected:
	virtual void ProcessJob(std::unique_ptr<IDBJob> upJob) override;
};
