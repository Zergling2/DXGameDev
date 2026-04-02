#include "GameRoom.h"
#include "GameSession.h"
#include <cassert>
#include "GameChannel.h"

GameRoom::GameRoom(uint64_t roomId, uint16_t gameNo, GameRoomMaxPlayer maxPlayer, GameMap gameMap, GameMode gameMode,
	const wchar_t* name, uint16_t nameLen)
	: m_gameRoomId(roomId)
	, m_gameRoomNo(gameNo)
	, m_maxPlayer(maxPlayer)
	, m_gameMap(gameMap)
	, m_gameMode(gameMode)
	, m_state(GameRoomState::InWaiting)
	, m_nameLen(0)
	, m_redTeamSessions()
	, m_blueTeamSessions()
	, m_redTeamSessionsCount(0)
	, m_blueTeamSessionsCount(0)
{
	this->SetName(name, nameLen);
}

void GameRoom::SetName(const wchar_t* name, uint16_t len)
{
	assert(len <= MAX_GAME_ROOM_NAME_LEN);

	m_nameLen = len;
	wmemcpy_s(m_name, _countof(m_name), name, len);
	m_name[len] = L'\0';
}

bool GameRoom::AddSession(std::shared_ptr<GameSession> spSession, GameTeam& team)
{
	const size_t maxPlayerForEachTeam = static_cast<size_t>(m_maxPlayer) + 1;

	if (m_redTeamSessionsCount + m_blueTeamSessionsCount == maxPlayerForEachTeam * 2)
		return false;

	// 빈 슬롯에 세션 포인터 저장
	if (m_redTeamSessionsCount < maxPlayerForEachTeam)
	{
		assert(m_redTeamSessions[m_redTeamSessionsCount] == nullptr);

		m_redTeamSessions[m_redTeamSessionsCount] = spSession;
		team = GameTeam::RedTeam;

		++m_redTeamSessionsCount;
	}
	else
	{
		// 블루 팀에 빈자리
		assert(m_blueTeamSessions[m_blueTeamSessionsCount] == nullptr);
		m_blueTeamSessions[m_blueTeamSessionsCount] = spSession;
		team = GameTeam::BlueTeam;

		++m_blueTeamSessionsCount;
	}

	return true;
}

void GameRoom::RemoveSession(uint64_t netId)
{
	size_t removalIndex = (std::numeric_limits<size_t>::max)();
	GameTeam removalTeam = GameTeam::Unknown;

	do
	{
		// 레드팀에서 세션 검색 및 제거
		for (size_t i = 0; i < _countof(m_redTeamSessions); ++i)
		{
			if (m_redTeamSessions[i]->GetNetId() == netId)
			{
				removalIndex = i;
				break;
			}
		}

		if (removalIndex != (std::numeric_limits<size_t>::max)())
		{
			const size_t lastIndex = m_redTeamSessionsCount - 1;
			if (removalIndex != lastIndex)
				std::swap(m_redTeamSessions[removalIndex], m_redTeamSessions[lastIndex]);

			m_redTeamSessions[lastIndex].reset();
			--m_redTeamSessionsCount;

			break;
		}

		// 블루팀에서 세션 검색 및 제거
		for (size_t i = 0; i < _countof(m_blueTeamSessions); ++i)
		{
			if (m_blueTeamSessions[i]->GetNetId() == netId)
			{
				removalIndex = i;
				break;
			}
		}
		
		if (removalIndex != (std::numeric_limits<size_t>::max)())
		{
			const size_t lastIndex = m_blueTeamSessionsCount - 1;
			if (removalIndex != lastIndex)
				std::swap(m_blueTeamSessions[removalIndex], m_blueTeamSessions[lastIndex]);

			m_blueTeamSessions[lastIndex].reset();
			--m_blueTeamSessionsCount;

			break;
		}

		assert(removalIndex != (std::numeric_limits<size_t>::max)());
	} while (false);
}
