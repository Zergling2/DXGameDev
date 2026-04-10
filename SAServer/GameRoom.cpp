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

bool GameRoom::AddSession(std::shared_ptr<GameSession> spSession)
{
	const size_t maxPlayerForEachTeam = static_cast<size_t>(m_maxPlayer) + 1;

	if (m_redTeamSessionsCount + m_blueTeamSessionsCount == maxPlayerForEachTeam * 2)
		return false;

	// 后 浇吩俊 技记 器牢磐 历厘
	if (m_redTeamSessionsCount < maxPlayerForEachTeam)
	{
		assert(m_redTeamSessions[m_redTeamSessionsCount] == nullptr);

		const uint8_t ti = m_redTeamSessionsCount;
		m_redTeamSessions[ti] = spSession;
		spSession->SetCurrTeam(GameTeam::RedTeam);
		spSession->SetCurrTeamIndex(ti);

		++m_redTeamSessionsCount;
	}
	else
	{
		// 喉风 评俊 后磊府
		assert(m_blueTeamSessions[m_blueTeamSessionsCount] == nullptr);

		const uint8_t ti = m_blueTeamSessionsCount;
		m_blueTeamSessions[ti] = spSession;
		spSession->SetCurrTeam(GameTeam::BlueTeam);
		spSession->SetCurrTeamIndex(ti);

		++m_blueTeamSessionsCount;
	}

	spSession->SetReadyState(false);

	return true;
}

void GameRoom::RemoveSession(std::shared_ptr<GameSession> spSession)
{
	assert(spSession->GetCurrTeam() != GameTeam::Unknown);

	const uint8_t removalIndex = spSession->GetCurrTeamIndex();

	if (spSession->GetCurrTeam() == GameTeam::RedTeam)
	{
		assert(m_redTeamSessions[spSession->GetCurrTeamIndex()] == spSession);
		assert(m_redTeamSessionsCount > 0);

		const uint8_t lastIndex = m_redTeamSessionsCount - 1;
		if (removalIndex != lastIndex)
		{
			std::swap(m_redTeamSessions[removalIndex], m_redTeamSessions[lastIndex]);
			m_redTeamSessions[removalIndex]->SetCurrTeamIndex(removalIndex);
		}

		m_redTeamSessions[lastIndex].reset();
		--m_redTeamSessionsCount;
	}
	else
	{
		assert(m_blueTeamSessions[spSession->GetCurrTeamIndex()] == spSession);
		assert(m_blueTeamSessionsCount > 0);

		const uint8_t lastIndex = m_blueTeamSessionsCount - 1;
		if (removalIndex != lastIndex)
		{
			std::swap(m_blueTeamSessions[removalIndex], m_blueTeamSessions[lastIndex]);
			m_blueTeamSessions[removalIndex]->SetCurrTeamIndex(removalIndex);
		}

		m_blueTeamSessions[lastIndex].reset();
		--m_blueTeamSessionsCount;
	}

	spSession->SetCurrTeam(GameTeam::Unknown);
	spSession->SetCurrTeamIndex((std::numeric_limits<uint8_t>::max)());
	spSession->SetReadyState(false);
}
