#include "DBThread.h"
#include <strsafe.h>

const char* HOST_NAME = "192.168.55.162:3306";
const char* USER_NAME = "sa_server";
const char* PASSWORD = ")5T_YvXk5D1@";
const char* SCHEMA = "sa_db";

DBThread::DBThread()
	: m_dbConn(sql::mysql::get_mysql_driver_instance())
{
}

void DBThread::OnThreadStart()
{
	bool result = m_dbConn.Connect(HOST_NAME, USER_NAME, PASSWORD, SCHEMA);

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
	m_dbConn.Release();
}

void DBThread::ProcessJob(std::unique_ptr<IDBJob> upJob)
{
	upJob->Execute(*this);
}

DBJobIdDuplicateCheck::DBJobIdDuplicateCheck(uint64_t netId, const char* id)
	: m_netId(netId)
{
	HRESULT hr = StringCchCopyA(m_id, _countof(m_id), id);

	m_isValid = SUCCEEDED(hr);
}

void DBJobIdDuplicateCheck::Execute(DBThread& thread)
{
}
