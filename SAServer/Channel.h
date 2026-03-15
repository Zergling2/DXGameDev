#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include <unordered_map>
#include <memory>
#include "SlimRWLock.h"

class Session;

class Channel
{
public:
	Channel();
	~Channel() = default;
private:
	SlimRWLock m_lock;
	std::unordered_map<uint64_t, std::shared_ptr<Session>> m_joinedSessions;
};
