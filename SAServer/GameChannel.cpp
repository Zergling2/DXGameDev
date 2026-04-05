#include "GameChannel.h"
#include "GameSession.h"
#include <cassert>

GameChannel::GameChannel()
	: m_hThread(NULL)
	, m_hJobEvent(NULL)
	, m_exitFlag(false)
	, m_channelId((std::numeric_limits<uint16_t>::max)())
	, m_jobQueueLock()
	, m_jobQueue()
	, m_sessions()
	, m_gameRoomIdCounter(0)
	, m_gameRoomNoCounter(0)
	, m_gameRooms()
{
}

int GameChannel::Init(uint16_t channelId)
{
	m_channelId = channelId;

	HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, GameChannel::ThreadEntry, this, 0, nullptr));
	if (hThread == NULL)
		return -1;

	HANDLE hJobEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (hJobEvent == NULL)
		return -1;

	m_hThread = hThread;
	m_hJobEvent = hJobEvent;

	return 0;
}

void GameChannel::Shutdown()
{
	if (m_hThread == NULL || m_hJobEvent == NULL)
		return;

	m_exitFlag = true;

	_ReadWriteBarrier();

	SetEvent(m_hJobEvent);

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hJobEvent);
	m_hJobEvent = NULL;

	CloseHandle(m_hThread);
	m_hThread = NULL;

	m_exitFlag = false;
}

void GameChannel::DispatchJob(std::unique_ptr<IChannelJob> upJob)
{
	m_jobQueueLock.AcquireLockExclusive();
	m_jobQueue.push(std::move(upJob));
	m_jobQueueLock.ReleaseLockExclusive();

	SetEvent(m_hJobEvent);
}

void GameChannel::AddSession(uint64_t netId, std::shared_ptr<GameSession> spSession)
{
	auto ret = m_sessions.insert(std::make_pair(netId, std::move(spSession)));
	assert(ret.second);
}

void GameChannel::RemoveSession(uint64_t netId)
{
	size_t ret = m_sessions.erase(netId);
	assert(ret);
}

unsigned int __stdcall GameChannel::ThreadEntry(void* pArg)
{
	GameChannel* pChannel = static_cast<GameChannel*>(pArg);
	volatile bool& exitFlag = pChannel->m_exitFlag;
	HANDLE hJobEvent = pChannel->m_hJobEvent;
	SlimRWLock& jobQueueLock = pChannel->m_jobQueueLock;
	std::queue<std::unique_ptr<IChannelJob>>& jobQueue = pChannel->m_jobQueue;

	while (!exitFlag)
	{
		WaitForSingleObject(hJobEvent, INFINITE);

		for (;;)
		{
			jobQueueLock.AcquireLockExclusive();

			if (jobQueue.empty())
			{
				jobQueueLock.ReleaseLockExclusive();
				break;
			}

			std::unique_ptr<IChannelJob> upJob = std::move(jobQueue.front());
			jobQueue.pop();

			jobQueueLock.ReleaseLockExclusive();

			upJob->Execute(*pChannel);
		}
	}
	
	return 0;
}
