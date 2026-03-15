#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <memory>
#include <vector>
#include "SlimRWLock.h"

class Session;

/**
* @brief 게임 방
*/
class Room
{
public:
	Room();
	virtual ~Room() = default;

	void JoinSession(uint64_t id);	// 해야할 일) 세션 상태 변경, 락 획득 및 m_joinedSessions에 항목 추가, 
	bool RemoveSession(uint64_t id);
private:
	SlimRWLock m_lock;
	std::vector<std::pair<uint64_t, std::shared_ptr<Session>>> m_joinedSessions;
};
