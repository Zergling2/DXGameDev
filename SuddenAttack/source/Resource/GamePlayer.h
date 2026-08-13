#pragma once

#include <string>
#include <cstdint>

class GamePlayer
{
public:
	GamePlayer();
	~GamePlayer();

	const wchar_t* GetName() const { return m_name.c_str(); }
	void SetName(const wchar_t* name) { m_name = name; }
	int32_t GetKill() const { return m_kill; }
	void SetKill(int32_t val) { m_kill = val; }
	int32_t GetDeath() const { return m_death; }
	void SetDeath(int32_t val) { m_death = val; }
	int32_t GetPint() const { return m_ping; }
	void SetPing(int32_t val) { m_ping = val; }
private:
	std::wstring m_name;
	int32_t m_kill;
	int32_t m_death;
	int32_t m_ping;
};
