#include "DBThread.h"

void DBThread::ProcessJob(std::unique_ptr<IDBJob> upJob)
{
	upJob->Execute();
}
