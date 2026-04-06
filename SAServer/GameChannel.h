#pragma once

#include <cstdint>
#include <unordered_map>
#include <memory>
#include <queue>
#include <thread>
#include "SlimRWLock.h"
#include "ChannelJob.h"

class GameSession;
class GameRoom;

class GameChannel
{
public:
	GameChannel();
	~GameChannel() = default;

	int Init(uint16_t channelId);
	void Shutdown();

	void DispatchJob(std::unique_ptr<IChannelJob> upJob);

	std::unordered_map<uint64_t, std::shared_ptr<GameSession>>& Sessions() { return m_sessions; }
	void AddSession(uint64_t netId, std::shared_ptr<GameSession> spSession);
	void RemoveSession(uint64_t netId);
	uint16_t GetChannelId() const { return m_channelId; }
	uint64_t CreateGameRoomId() { return InterlockedIncrement(&m_gameRoomIdCounter); }
	uint16_t CreateGameRoomNo() { return InterlockedIncrement16(reinterpret_cast<volatile short*>(&m_gameRoomNoCounter)); }
	size_t GetSessionCount() const { return m_sessions.size(); }
	std::unordered_map<uint64_t, std::shared_ptr<GameRoom>>& GameRooms() { return m_gameRooms; }
private:
	static unsigned int __stdcall ThreadEntry(void* pArg);
private:
	HANDLE m_hThread;
	mutable SRWLOCK m_jobQueueLock;
	CONDITION_VARIABLE m_jobConditionVar;
	volatile bool m_exitFlag;
	uint16_t m_channelId;
	std::queue<std::unique_ptr<IChannelJob>> m_jobQueue;
	std::unordered_map<uint64_t, std::shared_ptr<GameSession>> m_sessions;
	uint64_t m_gameRoomIdCounter;
	uint16_t m_gameRoomNoCounter;
	std::unordered_map<uint64_t, std::shared_ptr<GameRoom>> m_gameRooms;
};
