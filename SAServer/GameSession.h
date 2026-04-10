#pragma once

#include <memory>
#include "Constants.h"
#include "Contents.h"
#include "SlimRWLock.h"

class GameChannel;
class GameRoom;

class GameSession
{
public:
	GameSession(uint64_t netId, uint64_t accountId);
	~GameSession() = default;

	uint64_t GetNetId() const { return m_netId; }
	uint64_t GetAccountId() const { return m_accountId; }
	uint16_t GetNicknameLen() const { return m_nicknameLen; }
	void SetNickname(const wchar_t* nickname, uint16_t len);
	const wchar_t* GetNickname() const { return m_nickname; }
	uint16_t GetLevel() const { return m_level; }
	void SetLevel(uint16_t level) { m_level = level; }
	uint32_t GetExp() const { return m_exp; }
	void SetExp(uint32_t exp) { m_exp = exp; }
	uint32_t GetPoint() const { return m_point; }
	void SetPoint(uint32_t point) { m_point = point; }
	void SetDisconnectedFlag();
	bool GetDisconnectedFlag();
	GameChannel* GetJoiningGameChannel();
	void SetJoiningGameChannel(GameChannel* pChannel);
	std::shared_ptr<GameRoom> GetJoiningGameRoom() const { return m_wpJoiningGameRoom.lock(); }
	void SetJoiningGameRoom(std::shared_ptr<GameRoom> spRoom) { m_wpJoiningGameRoom = spRoom; }
	void SetCurrTeam(GameTeam team) { m_team = team; }
	GameTeam GetCurrTeam() const { return m_team; }
	void SetCurrTeamIndex(uint8_t index) { m_teamIndex = index; }
	uint8_t GetCurrTeamIndex() const { return m_teamIndex; }
	void SetReadyState(bool ready) { m_ready = ready; }
	bool GetReadyState() const { return m_ready; }
private:
	const uint64_t m_netId;
	const uint64_t m_accountId;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];	// 디버그 용도 출력 소요 등으로 인해 null termination string으로 보관.
	uint16_t m_level;	// 레벨
	uint32_t m_exp;		// 경험치
	uint32_t m_point;	// 포인트 소유량
	// #######################
	SlimRWLock m_channelLock;
	bool m_disconnected;
	GameChannel* m_pJoiningGameChannel;
	// #######################
	std::weak_ptr<GameRoom> m_wpJoiningGameRoom;
	GameTeam m_team;
	uint8_t m_teamIndex;
	bool m_ready;
};
