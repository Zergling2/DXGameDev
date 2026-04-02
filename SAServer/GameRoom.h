#pragma once

#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <memory>
#include <vector>
#include "Constants.h"
#include "Contents.h"

class GameSession;

class GameRoom
{
public:
	GameRoom(uint64_t roomId, uint16_t gameNo, GameRoomMaxPlayer maxPlayer, GameMap gameMap, GameMode gameMode,
		const wchar_t* name, uint16_t nameLen);
	virtual ~GameRoom() = default;

	uint64_t GetRoomId() const { return m_gameRoomId; }
	uint16_t GetRoomNo() const { return m_gameRoomNo; }
	GameRoomMaxPlayer GetMaxPlayer() const { return m_maxPlayer; }
	bool SetMaxPlayer(GameRoomMaxPlayer maxPlayer);
	GameMap GetGameMap() const { return m_gameMap; }
	void SetGameMap(GameMap map);
	GameMode GetGameMode() const { return m_gameMode; }
	void SetGameMode(GameMode mode);
	GameRoomState GetGameRoomState() const { return m_state; }
	void SetGameRoomState(GameRoomState state) { m_state = state; }
	uint16_t GetNameLen() const { return m_nameLen; }
	void SetName(const wchar_t* name, uint16_t len);
	const wchar_t* GetName() const { return m_name; }

	GameSession* GetRedTeamSessionPtr(size_t index) const { return m_redTeamSessions[index].get(); }
	GameSession* GetBlueTeamSessionPtr(size_t index) const { return m_blueTeamSessions[index].get(); }
	uint8_t GetRedTeamSessionsCount() const { return m_redTeamSessionsCount; }
	uint8_t GetBlueTeamSessionsCount() const { return m_blueTeamSessionsCount; }

	bool AddSession(std::shared_ptr<GameSession> spSession, GameTeam& team);
	void RemoveSession(uint64_t netId);
private:
	const uint64_t m_gameRoomId;
	const uint16_t m_gameRoomNo;
	GameRoomMaxPlayer m_maxPlayer;
	GameMap m_gameMap;
	GameMode m_gameMode;
	GameRoomState m_state;
	uint16_t m_nameLen;
	wchar_t m_name[MAX_GAME_ROOM_NAME_LEN + 1];
public:
	std::shared_ptr<GameSession> m_redTeamSessions[MAX_PLAYERS_PER_TEAM];
	std::shared_ptr<GameSession> m_blueTeamSessions[MAX_PLAYERS_PER_TEAM];
	uint8_t m_redTeamSessionsCount;
	uint8_t m_blueTeamSessionsCount;
	std::shared_ptr<GameSession> m_spHost;
};
