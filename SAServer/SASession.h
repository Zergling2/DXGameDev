#pragma once

#include "Constants.h"

class Channel;
class Room;

enum class SASessionState
{
	WaitingLogin,
	JoinedChannel,
	JoinedRoom
};

class SASession
{
public:
	SASession(uint64_t id)
		: m_netId(id)
		, m_authenticated(false)
		, m_nickname{ L'\0'}
		, m_level(0)
		, m_exp(0)
		, m_point(0)
		, m_state(SASessionState::WaitingLogin)
		, m_pParticipatingChannel(nullptr)
		, m_pParticipatingRoom(nullptr)
	{
	}

	uint64_t GetNetId() const { return m_netId; }
	bool GetAuthenticated() const { return m_authenticated; }
	void SetAuthenticated() { m_authenticated = true; }
	const wchar_t* GetNickname() const { return m_nickname; }
	void SetNickname(const wchar_t* nickname);
	uint16_t GetLevel() const { return m_level; }
	void SetLevel(uint16_t level) { m_level = level; }
	uint32_t GetExp() const { return m_exp; }
	void SetExp(uint32_t exp) { m_exp = exp; }
	uint32_t GetPoint() const { return m_point; }
	void SetPoint(uint32_t point) { m_point = point; }
	SASessionState GetState() const { return m_state; }
	Channel* GetParticipatingChannel() const { return m_pParticipatingChannel; }
	Room* GetParticipatingRoom() const { return m_pParticipatingRoom; }
private:
	uint64_t m_netId;
	bool m_authenticated;
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];	// null termination 형태로 저장
	uint16_t m_level;	// 레벨
	uint32_t m_exp;		// 경험치
	uint32_t m_point;	// 포인트 소유량

	SASessionState m_state;
	Channel* m_pParticipatingChannel;
	Room* m_pParticipatingRoom;
};
