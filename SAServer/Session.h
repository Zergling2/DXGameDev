#pragma once

#include <cstdint>

class Player;

class Session
{
public:
	explicit Session(uint64_t netId)
		: m_pPlayer(nullptr)
		, m_netId(netId)
	{
	}
	void BindPlayer(Player* pPlayer) { m_pPlayer = pPlayer; }
	void UnbindPlayer() { m_pPlayer = nullptr; }
	Player* GetPlayer() const { return m_pPlayer; }
	uint64_t GetNetId() const { return m_netId; }
private:
	Player* m_pPlayer;
	uint64_t m_netId;
};
