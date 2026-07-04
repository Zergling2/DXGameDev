#include "GameRoom.h"
#include <cassert>
#include "Player.h"
#include "Session.h"
#include "Protocol.h"

GameRoom::GameRoom(uint64_t id, uint16_t no, GameRoomTeamFormat tf, GameMap map, const wchar_t* name)
	: m_id(id)
	, m_no(no)
	, m_tf(tf)
	, m_map(map)
	, m_state(GameRoomState::InWaiting)
	, m_name(name)
	, m_redTeam()
	, m_blueTeam()
	, m_pHost(nullptr)
{
}

bool GameRoom::AddPlayerAsHost(Player* pPlayer, GameTeam& team)
{
	if (!pPlayer)
		return false;

	if (this->IsFull())
		return false;

	// 오토 팀밸런스
	const size_t maxPlayersForEachTeam = GameRoomTeamFormatToMaxPlayerCount(m_tf) / 2;
	if (m_redTeam.size() < maxPlayersForEachTeam && m_blueTeam.size() < maxPlayersForEachTeam)
	{
		if (m_redTeam.size() <= m_blueTeam.size())
		{
			m_redTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
			team = GameTeam::RedTeam;
		}
		else
		{
			m_blueTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
			team = GameTeam::BlueTeam;
		}
	}
	else
	{
		if (m_redTeam.size() < maxPlayersForEachTeam)
		{
			m_redTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
			team = GameTeam::RedTeam;
		}
		else
		{
			m_blueTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
			team = GameTeam::BlueTeam;
		}
	}

	pPlayer->MarkEnterRoom(this->m_id);

	m_pHost = pPlayer;

	return true;
}

bool GameRoom::AddPlayer(Player* pPlayer, GameTeam& team)
{
	if (!pPlayer)
		return false;

	if (this->IsFull())
		return false;

	const size_t maxPlayersForEachTeam = GameRoomTeamFormatToMaxPlayerCount(m_tf) / 2;
	if (m_redTeam.size() < maxPlayersForEachTeam && m_blueTeam.size() < maxPlayersForEachTeam)
	{
		if (m_redTeam.size() <= m_blueTeam.size())
		{
			m_redTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
			team = GameTeam::RedTeam;
		}
		else
		{
			m_blueTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
			team = GameTeam::BlueTeam;
		}
	}
	else
	{
		if (m_redTeam.size() < maxPlayersForEachTeam)
		{
			m_redTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
			team = GameTeam::RedTeam;
		}
		else
		{
			m_blueTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
			team = GameTeam::BlueTeam;
		}
	}

	pPlayer->MarkEnterRoom(this->m_id);

	return true;
}

bool GameRoom::MoveTeam(const Player* pPlayer, GameTeam newTeam)
{
	assert(newTeam == GameTeam::RedTeam || newTeam == GameTeam::BlueTeam);

	GameTeam team;
	size_t index;
	PlayerState state;

	if (!FindPlayer(pPlayer->GetAccountId(), team, index, state))
		return false;

	if (team == newTeam)
		return false;

	const size_t maxPlayerPerTeam = GameRoomTeamFormatToMaxPlayerCount(m_tf) / 2;
	if (newTeam == GameTeam::RedTeam)
	{
		if (m_redTeam.size() >= maxPlayerPerTeam)
			return false;
	}
	else if (newTeam == GameTeam::BlueTeam)
	{
		if (m_blueTeam.size() >= maxPlayerPerTeam)
			return false;
	}
	else
	{
		return false;
	}

	if (team == GameTeam::RedTeam)
	{
		m_redTeam.erase(m_redTeam.begin() + index);
	}
	else if (team == GameTeam::BlueTeam)
	{
		m_blueTeam.erase(m_blueTeam.begin() + index);
	}

	if (newTeam == GameTeam::RedTeam)
	{
		m_redTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
	}
	else if (newTeam == GameTeam::BlueTeam)
	{
		m_blueTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
	}


	return true;
}

RemovePlayerResult GameRoom::RemovePlayer(Player* pPlayer)
{
	if (!pPlayer)
		return RemovePlayerResult::Normal;

	GameTeam team;
	size_t index;
	PlayerState state;
	if (!FindPlayer(pPlayer->GetAccountId(), team, index, state))
		return RemovePlayerResult::Normal;

	switch (team)
	{
	case GameTeam::RedTeam:
		m_redTeam.erase(m_redTeam.begin() + index);
		break;
	case GameTeam::BlueTeam:
		m_blueTeam.erase(m_blueTeam.begin() + index);
		break;
	default:
		break;
	}

	pPlayer->MarkExitRoom();

	if (m_pHost == pPlayer)
	{
		m_pHost = FindNewHost();

		if (m_pHost == nullptr)
			return RemovePlayerResult::LastPlayerRemoved;
		else
			return RemovePlayerResult::HostChanged;
	}

	if (m_redTeam.size() + m_blueTeam.size() == 0)
		return RemovePlayerResult::LastPlayerRemoved;

	return RemovePlayerResult::Normal;
}

void GameRoom::BroadcastPacket(winppy::TCPServer& server, winppy::Packet packet) const
{
	for (size_t i = 0; i < m_redTeam.size(); ++i)
		server.Send(m_redTeam[i].m_pPlayer->GetSession()->GetNetId(), packet);

	for (size_t i = 0; i < m_blueTeam.size(); ++i)
		server.Send(m_blueTeam[i].m_pPlayer->GetSession()->GetNetId(), packet);
}

void GameRoom::BroadcastPacketExceptPlayer(winppy::TCPServer& server, winppy::Packet packet, const Player* pPlayer) const
{
	for (size_t i = 0; i < m_redTeam.size(); ++i)
	{
		if (m_redTeam[i].m_pPlayer == pPlayer)
			continue;

		server.Send(m_redTeam[i].m_pPlayer->GetSession()->GetNetId(), packet);
	}

	for (size_t i = 0; i < m_blueTeam.size(); ++i)
	{
		if (m_blueTeam[i].m_pPlayer == pPlayer)
			continue;

		server.Send(m_blueTeam[i].m_pPlayer->GetSession()->GetNetId(), packet);
	}
}

winppy::Packet GameRoom::CreateGameRoomPlayerJoinedPacket(const Player* pPlayer) const
{
	GameTeam joinedTeam;
	size_t index;
	PlayerState state;

	if (!FindPlayer(pPlayer->GetAccountId(), joinedTeam, index, state))
		*reinterpret_cast<int*>(0) = 0;

	SCNotifyPlayerJoined notifyNewPlayer;
	notifyNewPlayer.m_accountId = pPlayer->GetAccountId();
	notifyNewPlayer.m_team = joinedTeam;
	notifyNewPlayer.m_level = pPlayer->GetLevel();
	notifyNewPlayer.m_state = state;
	notifyNewPlayer.m_nicknameLen = pPlayer->GetNicknameLen();
	wmemcpy_s(notifyNewPlayer.m_nickname, _countof(notifyNewPlayer.m_nickname), pPlayer->GetNickname(), notifyNewPlayer.m_nicknameLen);

	winppy::Packet pktNotifyNewPlayer;
	pktNotifyNewPlayer->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_JOINED_GAME_ROOM));
	pktNotifyNewPlayer->WriteBytes(&notifyNewPlayer, sizeof(notifyNewPlayer));

	return pktNotifyNewPlayer;
}

std::vector<winppy::Packet> GameRoom::CreateGameRoomPlayerPacketsExcept(const Player* pPlayer) const
{
	std::vector<winppy::Packet> pkts;

	for (size_t i = 0; i < m_redTeam.size(); ++i)
	{
		const GameRoomPlayer& gameRoomPlayer = m_redTeam[i];
		if (gameRoomPlayer.m_pPlayer == pPlayer)
			continue;

		SCNotifyGameRoomPlayer notifyExistPlayer;
		notifyExistPlayer.m_accountId = gameRoomPlayer.m_pPlayer->GetAccountId();
		notifyExistPlayer.m_team = GameTeam::RedTeam;
		notifyExistPlayer.m_level = gameRoomPlayer.m_pPlayer->GetLevel();
		notifyExistPlayer.m_state = gameRoomPlayer.m_state;
		notifyExistPlayer.m_nicknameLen = gameRoomPlayer.m_pPlayer->GetNicknameLen();
		wmemcpy_s(notifyExistPlayer.m_nickname, _countof(notifyExistPlayer.m_nickname), gameRoomPlayer.m_pPlayer->GetNickname(), notifyExistPlayer.m_nicknameLen);

		winppy::Packet pkt;
		pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_GAME_ROOM_PLAYER));
		pkt->WriteBytes(&notifyExistPlayer, sizeof(notifyExistPlayer));
		pkts.push_back(std::move(pkt));
	}

	for (size_t i = 0; i < m_blueTeam.size(); ++i)
	{
		const GameRoomPlayer& gameRoomPlayer = m_blueTeam[i];
		if (gameRoomPlayer.m_pPlayer == pPlayer)
			continue;

		SCNotifyGameRoomPlayer notifyExistPlayer;
		notifyExistPlayer.m_accountId = gameRoomPlayer.m_pPlayer->GetAccountId();
		notifyExistPlayer.m_team = GameTeam::BlueTeam;
		notifyExistPlayer.m_level = gameRoomPlayer.m_pPlayer->GetLevel();
		notifyExistPlayer.m_state = gameRoomPlayer.m_state;
		notifyExistPlayer.m_nicknameLen = gameRoomPlayer.m_pPlayer->GetNicknameLen();
		wmemcpy_s(notifyExistPlayer.m_nickname, _countof(notifyExistPlayer.m_nickname), gameRoomPlayer.m_pPlayer->GetNickname(), notifyExistPlayer.m_nicknameLen);

		winppy::Packet pkt;
		pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_GAME_ROOM_PLAYER));
		pkt->WriteBytes(&notifyExistPlayer, sizeof(notifyExistPlayer));
		pkts.push_back(std::move(pkt));
	}

	return pkts;
}

bool GameRoom::ChangePlayerState(uint32_t accountId, PlayerState newState)
{
	GameTeam team;
	size_t index;
	PlayerState state;

	if (!FindPlayer(accountId, team, index, state))
		return false;

	switch (team)
	{
	case GameTeam::RedTeam:
		m_redTeam[index].m_state = newState;
		break;
	case GameTeam::BlueTeam:
		m_blueTeam[index].m_state = newState;
		break;
	default:
		return false;
	}

	return true;
}

PlayerState GameRoom::GetPlayerState(uint32_t accountId) const
{
	GameTeam team;
	size_t index;
	PlayerState state;

	if (FindPlayer(accountId, team, index, state))
	{
		return state;
	}
	else
	{
		return PlayerState::Unknown;
	}
}

bool GameRoom::ChangePlayerTeam(uint32_t accountId, GameTeam newTeam)
{
	GameTeam oldTeam;
	size_t index;
	PlayerState state;

	if (!FindPlayer(accountId, oldTeam, index, state))
		return false;

	if (oldTeam == newTeam)
		return false;

	// 상대편으로 넘어갈 수 있는지 인원 수 체크
	const size_t maxPlayersForEachTeam = GameRoomTeamFormatToMaxPlayerCount(m_tf) / 2;
	if (newTeam == GameTeam::RedTeam)
	{
		if (m_redTeam.size() == maxPlayersForEachTeam)
			return false;
	}
	else if (newTeam == GameTeam::BlueTeam)
	{
		if (m_blueTeam.size() == maxPlayersForEachTeam)
			return false;
	}

	GameRoomPlayer player;

	switch (oldTeam)
	{
	case GameTeam::RedTeam:
		player = m_redTeam[index];
		m_redTeam.erase(m_redTeam.begin() + index);
		break;
	case GameTeam::BlueTeam:
		player = m_blueTeam[index];
		m_blueTeam.erase(m_blueTeam.begin() + index);
		break;
	default:
		break;
	}

	switch (newTeam)
	{
	case GameTeam::RedTeam:
		m_redTeam.push_back(player);
		break;
	case GameTeam::BlueTeam:
		m_blueTeam.push_back(player);
		break;
	default:
		break;
	}

	return true;
}

bool GameRoom::IsFull() const
{
	if (m_redTeam.size() + m_blueTeam.size() >= GameRoomTeamFormatToMaxPlayerCount(m_tf))
		return true;
	else
		return false;
}

bool GameRoom::FindPlayer(uint32_t accountId, GameTeam& team, size_t& index, PlayerState& state) const
{
	bool result = false;

	for (size_t i = 0; i < m_redTeam.size(); ++i)
	{
		const auto& gameRoomPlayer = m_redTeam[i];
		if (gameRoomPlayer.m_pPlayer->GetAccountId() == accountId)
		{
			team = GameTeam::RedTeam;
			index = i;
			state = gameRoomPlayer.m_state;
			result = true;
			break;
		}
	}

	for (size_t i = 0; i < m_blueTeam.size(); ++i)
	{
		const auto& gameRoomPlayer = m_blueTeam[i];
		if (gameRoomPlayer.m_pPlayer->GetAccountId() == accountId)
		{
			team = GameTeam::BlueTeam;
			index = i;
			state = gameRoomPlayer.m_state;
			result = true;
			break;
		}
	}

	return result;
}

const Player* GameRoom::FindNewHost() const
{
	if (!m_redTeam.empty())
		return m_redTeam.front().m_pPlayer;

	if (!m_blueTeam.empty())
		return m_blueTeam.front().m_pPlayer;

	return nullptr;
}
