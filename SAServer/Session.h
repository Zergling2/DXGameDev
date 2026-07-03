#pragma once

#include <cstddef>
#include <cstdint>

class Player;

class Session
{
public:
	explicit Session(uint64_t netId)
		: m_pPlayer(nullptr)
		, m_netId(netId)
		, m_dbThreadIndex(0)
	{
	}
	void BindPlayer(Player* pPlayer) { m_pPlayer = pPlayer; }
	void UnbindPlayer() { m_pPlayer = nullptr; }
	Player* GetPlayer() const { return m_pPlayer; }
	uint64_t GetNetId() const { return m_netId; }
	size_t GetDBThreadIndex() const { return m_dbThreadIndex; }
	void SetDBThreadIndex(size_t index) { m_dbThreadIndex = index; }
private:
	Player* m_pPlayer;
	uint64_t m_netId;
	size_t m_dbThreadIndex;
};
