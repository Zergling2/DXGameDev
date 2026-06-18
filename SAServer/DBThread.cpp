#include "DBThread.h"

void DBThread::OnThreadStart()
{
}

void DBThread::OnThreadExit()
{
}

void DBThread::ProcessJob(std::unique_ptr<IDBJob> upJob)
{
	upJob->Execute(*this);
}
