#pragma once

#include "JobThread.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>
#include <queue>
#include <memory>
#include <utility>

template<typename JobType>
class JobThreadBase : public IJobThread
{
public:
    JobThreadBase();
    virtual ~JobThreadBase();

    bool Start() override;
    void Shutdown() override;

    bool DispatchJob(std::unique_ptr<JobType> upJob);
protected:
    virtual void OnThreadStart() {}
    virtual void OnThreadExit() {}
    virtual void ProcessJob(std::unique_ptr<JobType> upJob) = 0;
private:
    static unsigned int __stdcall ThreadEntry(void* arg);
    void Run();
private:
    HANDLE m_hThread;
    SRWLOCK m_jobQueueLock;
    CONDITION_VARIABLE m_jobConditionVar;
    volatile bool m_shutdown;
    std::queue<std::unique_ptr<JobType>> m_jobQueue;
};

template<typename JobType>
JobThreadBase<JobType>::JobThreadBase()
    : m_hThread(NULL)
    , m_shutdown(false)
{
    InitializeSRWLock(&m_jobQueueLock);
    InitializeConditionVariable(&m_jobConditionVar);
}

template<typename JobType>
JobThreadBase<JobType>::~JobThreadBase()
{
    Shutdown();
}

template<typename JobType>
bool JobThreadBase<JobType>::Start()
{
    m_shutdown = false;

    unsigned int threadId;
    m_hThread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &ThreadEntry, this, 0, &threadId));

    return m_hThread != nullptr;
}

template<typename JobType>
void JobThreadBase<JobType>::Shutdown()
{
    if (m_hThread == NULL)
        return;

    AcquireSRWLockExclusive(&m_jobQueueLock);
    m_shutdown = true;
    MemoryBarrier();

    WakeAllConditionVariable(&m_jobConditionVar);

    ReleaseSRWLockExclusive(&m_jobQueueLock);

    WaitForSingleObject(m_hThread, INFINITE);

    CloseHandle(m_hThread);
    m_hThread = NULL;
}

template<typename JobType>
bool JobThreadBase<JobType>::DispatchJob(std::unique_ptr<JobType> upJob)
{
    AcquireSRWLockExclusive(&m_jobQueueLock);

    if (m_shutdown)
    {
        ReleaseSRWLockExclusive(&m_jobQueueLock);
        return false;
    }
    else
    {
        m_jobQueue.push(std::move(upJob));
        WakeConditionVariable(&m_jobConditionVar);
        ReleaseSRWLockExclusive(&m_jobQueueLock);
        return true;
    }
}

template<typename JobType>
unsigned int __stdcall JobThreadBase<JobType>::ThreadEntry(void* pArg)
{
    auto* pThis = static_cast<JobThreadBase<JobType>*>(pArg);

    pThis->Run();

    return 0;
}

template<typename JobType>
void JobThreadBase<JobType>::Run()
{
    this->OnThreadStart();

    while (true)
    {
        AcquireSRWLockExclusive(&m_jobQueueLock);

        while (m_jobQueue.empty() && !m_shutdown)
            SleepConditionVariableSRW(&m_jobConditionVar, &m_jobQueueLock, INFINITE, 0);

        // 여기서 스레드 종료로 이어질 때 추가된 잡은 수행되지 않을 수 있음.
        // 해결 방법은 잡 스레드가 종료에 돌입했는지를 확인하는 변수를 추가하고 그것 또한 m_jobQueueLock으로 보호하면 될 듯.
        if (m_jobQueue.empty() && m_shutdown)
        {
            ReleaseSRWLockExclusive(&m_jobQueueLock);
            break;
        }

        std::unique_ptr<JobType> upJob = std::move(m_jobQueue.front());
        m_jobQueue.pop();

        ReleaseSRWLockExclusive(&m_jobQueueLock);

        ProcessJob(std::move(upJob));
    }

    this->OnThreadExit();
}
