#include "GameChannel.h"
#include "GameSession.h"
#include <cassert>

GameChannel::GameChannel()
	: m_hThread(NULL)
	, m_jobQueueLock()
	, m_jobConditionVar()
	, m_exitFlag(false)
	, m_channelId((std::numeric_limits<uint16_t>::max)())
	, m_jobQueue()
	, m_sessions()
	, m_gameRoomIdCounter(0)
	, m_gameRoomNoCounter(0)
	, m_gameRooms()
{
}

int GameChannel::Init(uint16_t channelId)
{
	m_exitFlag = false;
	m_channelId = channelId;

	HANDLE hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, GameChannel::ThreadEntry, this, 0, nullptr));
	if (hThread == NULL)
		return -1;

	m_jobQueueLock = SRWLOCK_INIT;
	m_jobConditionVar = CONDITION_VARIABLE_INIT;

	m_hThread = hThread;

	return 0;
}

void GameChannel::Shutdown()
{
	if (m_hThread == NULL)
		return;

	AcquireSRWLockExclusive(&m_jobQueueLock);

	m_exitFlag = true;
	_ReadWriteBarrier();

	WakeAllConditionVariable(&m_jobConditionVar);

	ReleaseSRWLockExclusive(&m_jobQueueLock);

	WaitForSingleObject(m_hThread, INFINITE);

	CloseHandle(m_hThread);
	m_hThread = NULL;

	m_exitFlag = false;
}

void GameChannel::DispatchJob(std::unique_ptr<IChannelJob> upJob)
{
	AcquireSRWLockExclusive(&m_jobQueueLock);
	m_jobQueue.push(std::move(upJob));

	WakeConditionVariable(&m_jobConditionVar);	// 락 점유 상태로

	ReleaseSRWLockExclusive(&m_jobQueueLock);
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
	SRWLOCK& jobQueueLock = pChannel->m_jobQueueLock;
	CONDITION_VARIABLE& jobConditionVar = pChannel->m_jobConditionVar;
	std::queue<std::unique_ptr<IChannelJob>>& jobQueue = pChannel->m_jobQueue;

	AcquireSRWLockExclusive(&jobQueueLock);

	for (;;)
	{
		// 조건이 만족될 때까지 대기
		while (jobQueue.empty() && !exitFlag)
		{
			SleepConditionVariableSRW(
				&jobConditionVar,
				&jobQueueLock,
				INFINITE,
				0
			);
		}

		// 종료 조건
		if (jobQueue.empty() && exitFlag)
		{
			ReleaseSRWLockExclusive(&jobQueueLock);
			goto EXIT_LOOP;
		}

		// 작업 꺼내기
		auto job = std::move(jobQueue.front());
		jobQueue.pop();

		ReleaseSRWLockExclusive(&jobQueueLock);
		job->Execute(*pChannel);
		AcquireSRWLockExclusive(&jobQueueLock);
	}
EXIT_LOOP:

	return 0;
}
