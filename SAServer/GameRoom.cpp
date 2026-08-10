#include "GameRoom.h"
#include <cassert>
#include "Player.h"
#include "Session.h"
#include "Protocol.h"

GameRoom::GameRoom(uint64_t id, uint16_t no, GameRoomTeamFormat tf, GameMap map, const wchar_t* name)
	: m_id(id)
	, m_no(no)
	, m_state(GameRoomState::InWaiting)
	, m_tf(tf)
	, m_map(map)
	, m_name(name)
	, m_redTeam()
	, m_blueTeam()
	, m_pHost(nullptr)
{
}

void GameRoom::SetState(winppy::TCPServer& server, GameRoomState state)
{
	m_state = state;

	SCNotifyGameRoomStateChanged noti;
	noti.m_gameRoomId = this->GetId();
	noti.m_newState = this->GetState();

	winppy::Packet pkt;
	pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_GAME_ROOM_STATE_CHANGED));
	pkt->WriteBytes(&noti, sizeof(noti));

	BroadcastPacket(server, std::move(pkt));
}

void GameRoom::AddPlayerAsHost(winppy::TCPServer& server, Player* pPlayer)
{
	assert(pPlayer != nullptr);
	assert(this->IsFull() == false);

	// 오토 팀밸런스
	const size_t maxPlayersForEachTeam = GameRoomTeamFormatToMaxPlayerCount(m_tf) / 2;
	GameTeam joinedTeam = GameTeam::Unknown;

	if (m_redTeam.size() < maxPlayersForEachTeam)
	{
		m_redTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
		joinedTeam = GameTeam::RedTeam;
	}
	else if (m_blueTeam.size() < maxPlayersForEachTeam)
	{
		m_blueTeam.push_back(GameRoomPlayer(pPlayer, PlayerState::None));
		joinedTeam = GameTeam::BlueTeam;
	}
	else
	{
		*reinterpret_cast<int*>(0) = 0;
	}

	assert(joinedTeam != GameTeam::Unknown);

	m_pHost = pPlayer;
	pPlayer->MarkEnterRoom(this->GetId());


	// 방 생성 및 방장으로 입장 성공 패킷 전송
	SCResCreateGameRoom res;
	res.m_result = true;
	res.m_gameRoomId = this->GetId();
	res.m_gameRoomNo = this->GetNo();
	res.m_gameRoomState = this->GetState();
	res.m_gameRoomHostAccountId = this->GetHost()->GetAccountId();
	res.m_gameRoomTeamFormat = this->GetTeamFormat();
	res.m_gameMap = this->GetMap();
	res.m_joinedTeam = joinedTeam;
	res.m_gameRoomNameLen = static_cast<uint16_t>(this->GetName().length());
	wmemcpy_s(res.m_gameRoomName, _countof(res.m_gameRoomName), this->GetName().c_str(), this->GetName().length());

	winppy::Packet pkt;
	pkt->Write(static_cast<protocol_type>(Protocol::SC_RES_CREATE_GAME_ROOM));
	pkt->WriteBytes(&res, sizeof(res));

	server.Send(pPlayer->GetSession()->GetNetId(), std::move(pkt));
}

bool GameRoom::AddPlayer(winppy::TCPServer& server, Player* pPlayer)
{
	assert(m_pHost != nullptr);
	assert(pPlayer != nullptr);
	assert(!this->IsFull());

	const size_t maxPlayersForEachTeam = GameRoomTeamFormatToMaxPlayerCount(m_tf) / 2;


	GameTeam joinedTeam = GameTeam::Unknown;
	PlayerState joinedState = PlayerState::None;

	if (m_redTeam.size() < maxPlayersForEachTeam && m_blueTeam.size() < maxPlayersForEachTeam)
	{
		if (m_redTeam.size() <= m_blueTeam.size())
		{
			m_redTeam.push_back(GameRoomPlayer(pPlayer, joinedState));
			joinedTeam = GameTeam::RedTeam;
		}
		else
		{
			m_blueTeam.push_back(GameRoomPlayer(pPlayer, joinedState));
			joinedTeam = GameTeam::BlueTeam;
		}
	}
	else
	{
		if (m_redTeam.size() < maxPlayersForEachTeam)
		{
			m_redTeam.push_back(GameRoomPlayer(pPlayer, joinedState));
			joinedTeam = GameTeam::RedTeam;
		}
		else
		{
			m_blueTeam.push_back(GameRoomPlayer(pPlayer, joinedState));
			joinedTeam = GameTeam::BlueTeam;
		}
	}

	assert(joinedTeam != GameTeam::Unknown);

	pPlayer->MarkEnterRoom(this->m_id);

	{
		// 방에 입장한 플레이어에게 성공 패킷을 전송. (방의 기본정보 & 입장 정보들도 함께 전송)
		SCResJoinGameRoom res;
		res.m_result = JoinGameRoomResult::Success;
		res.m_gameRoomId = this->GetId();
		res.m_gameRoomNo = this->GetNo();
		res.m_gameRoomState = this->GetState();
		res.m_gameRoomHostAccountId = this->GetHost()->GetAccountId();
		res.m_gameRoomTeamFormat = this->GetTeamFormat();
		res.m_gameMap = this->GetMap();
		res.m_joinedTeam = joinedTeam;
		res.m_gameRoomNameLen = static_cast<uint16_t>(this->GetName().length());
		wmemcpy_s(res.m_gameRoomName, _countof(res.m_gameRoomName), this->GetName().c_str(), res.m_gameRoomNameLen);

		winppy::Packet pktResJoinGameRoom;
		pktResJoinGameRoom->Write(static_cast<protocol_type>(Protocol::SC_RES_JOIN_GAME_ROOM));
		pktResJoinGameRoom->WriteBytes(&res, sizeof(res));
		server.Send(pPlayer->GetSession()->GetNetId(), std::move(pktResJoinGameRoom));
	}


	{
		// 지금 들어간 플레이어에게 방에 존재하던 플레이어들 정보 전송
		const uint64_t joinedSessionNetId = pPlayer->GetSession()->GetNetId();

		for (size_t i = 0; i < m_redTeam.size(); ++i)
		{
			const GameRoomPlayer& gameRoomPlayer = m_redTeam[i];
			if (gameRoomPlayer.m_pPlayer == pPlayer)		// 새로 입장한 플레이어에게 자기 자신 정보를 보내지 않기 위함.
				continue;

			SCNotifyGameRoomPlayer noti;
			noti.m_accountId = gameRoomPlayer.m_pPlayer->GetAccountId();
			noti.m_team = GameTeam::RedTeam;
			noti.m_level = gameRoomPlayer.m_pPlayer->GetLevel();
			noti.m_state = gameRoomPlayer.m_state;
			noti.m_nicknameLen = gameRoomPlayer.m_pPlayer->GetNicknameLen();
			wmemcpy_s(noti.m_nickname, _countof(noti.m_nickname), gameRoomPlayer.m_pPlayer->GetNickname(), noti.m_nicknameLen);

			winppy::Packet pkt;
			pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_GAME_ROOM_PLAYER));
			pkt->WriteBytes(&noti, sizeof(noti));

			server.Send(joinedSessionNetId, std::move(pkt));
		}

		for (size_t i = 0; i < m_blueTeam.size(); ++i)
		{
			const GameRoomPlayer& gameRoomPlayer = m_blueTeam[i];
			if (gameRoomPlayer.m_pPlayer == pPlayer)		// 새로 입장한 플레이어에게 자기 자신 정보를 보내지 않기 위함.
				continue;

			SCNotifyGameRoomPlayer noti;
			noti.m_accountId = gameRoomPlayer.m_pPlayer->GetAccountId();
			noti.m_team = GameTeam::BlueTeam;
			noti.m_level = gameRoomPlayer.m_pPlayer->GetLevel();
			noti.m_state = gameRoomPlayer.m_state;
			noti.m_nicknameLen = gameRoomPlayer.m_pPlayer->GetNicknameLen();
			wmemcpy_s(noti.m_nickname, _countof(noti.m_nickname), gameRoomPlayer.m_pPlayer->GetNickname(), noti.m_nicknameLen);

			winppy::Packet pkt;
			pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_GAME_ROOM_PLAYER));
			pkt->WriteBytes(&noti, sizeof(noti));

			server.Send(joinedSessionNetId, std::move(pkt));
		}
	}

	
	

	// 먼저 들어가있던 사람들에게 새로 입장한 플레이어의 정보 전송
	{
		SCNotifyPlayerJoined notifyNewPlayer;
		notifyNewPlayer.m_accountId = pPlayer->GetAccountId();
		notifyNewPlayer.m_team = joinedTeam;
		notifyNewPlayer.m_level = pPlayer->GetLevel();
		notifyNewPlayer.m_state = joinedState;
		notifyNewPlayer.m_nicknameLen = pPlayer->GetNicknameLen();
		wmemcpy_s(notifyNewPlayer.m_nickname, _countof(notifyNewPlayer.m_nickname), pPlayer->GetNickname(), notifyNewPlayer.m_nicknameLen);

		winppy::Packet pkt;
		pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_JOINED_GAME_ROOM));
		pkt->WriteBytes(&notifyNewPlayer, sizeof(notifyNewPlayer));

		this->BroadcastPacketExcept(server, std::move(pkt), pPlayer->GetAccountId());
	}

	return true;
}

size_t GameRoom::RemovePlayer(winppy::TCPServer& server, Player* pPlayer)
{
	GameTeam team;
	size_t index;
	PlayerState state;
	if (!FindPlayer(pPlayer->GetAccountId(), team, index, state))
		*reinterpret_cast<int*>(0) = 0;

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

	pPlayer->MarkExitRoom();	// 팀에서 제거 및 방 입장 플래그 제거

	{
		// ######################################################
		// 나간 플레이어에게 전달되는 패킷
		SCResExitGameRoom res;
		res.m_result = true;
		winppy::Packet pkt;
		pkt->Write(static_cast<protocol_type>(Protocol::SC_RES_EXIT_GAME_ROOM));
		pkt->WriteBytes(&res, sizeof(res));
		server.Send(pPlayer->GetSession()->GetNetId(), pkt);
		// ######################################################
	}

	{
		// ######################################################
		// 나간 플레이어를 제외한 나머지 플레이어들에게 전달되는 패킷
		SCNotifyPlayerExitGameRoom notifyPlayerExitRoom;
		notifyPlayerExitRoom.m_accountId = pPlayer->GetAccountId();

		winppy::Packet pkt;
		pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_EXIT_GAME_ROOM));
		pkt->WriteBytes(&notifyPlayerExitRoom, sizeof(notifyPlayerExitRoom));
		this->BroadcastPacket(server, pkt);
		// ######################################################
	}



	do
	{
		// 만약 나가는 플레이어가 방장이었던 경우
		if (m_pHost == pPlayer)
		{
			m_pHost = SelectRandomPlayer();		// 새로운 방장 결정
			if (m_pHost == nullptr)
				break;



			// 새로운 방장은 대기실 상태를 준비 해제 상태(PlayerState::None)으로 설정.
			// 만약 정비중이었던 경우에는 예외적으로 상태를 변경하지 않는다.
			GameTeam newHostTeam = GameTeam::Unknown;
			size_t newHostIndex = 0;
			PlayerState newHostState = PlayerState::Unknown;
			assert(FindPlayer(this->GetHost()->GetAccountId(), newHostTeam, newHostIndex, newHostState));

			if (newHostState == PlayerState::Ready)	// 준비중이었다면 준비 해제 상태(방장은 준비 상태 X), 정비중이거나 준비 해제 상태였으면 그대로.
				ChangePlayerState(server, this->GetHost()->GetAccountId(), PlayerState::None);


			// 방장 위임 등의 상황에도 이 패킷으로 대응하기 위해 이전 방장의 변경된 정보까지 전달
			SCNotifyHostChanged notifyHostChanged;
			notifyHostChanged.m_oldHostAccountId = pPlayer->GetAccountId();
			notifyHostChanged.m_oldHostNewState = PlayerState::Unknown;	// 방장 위임 등으로 바뀐게 아닌 방장이 방을 나가버린 경우 Unknown 처리됨.
			notifyHostChanged.m_newHostAccountId = this->GetHost()->GetAccountId();
			notifyHostChanged.m_newHostNewState = this->GetPlayerState(this->GetHost()->GetAccountId());

			winppy::Packet pkt;
			pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_HOST_CHANGED));
			pkt->WriteBytes(&notifyHostChanged, sizeof(notifyHostChanged));
			this->BroadcastPacket(server, pkt);
		}
	} while (false);

	return this->GetNumOfPlayers();
}

HostGameStartableResult GameRoom::IsGameStartable() const
{
	HostGameStartableResult result = HostGameStartableResult::Unknown;

	switch (this->GetState())
	{
	case GameRoomState::InWaiting:
	{
		GameTeam hostTeam;
		size_t hostIndex;
		PlayerState hostState;
		this->FindPlayer(m_pHost->GetAccountId(), hostTeam, hostIndex, hostState);

		const auto& opposingTeam = hostTeam == GameTeam::RedTeam ? m_blueTeam : m_redTeam;
		bool opposingReadyExist = false;
		for (auto& item : opposingTeam)
		{
			if (item.m_state == PlayerState::Ready)
			{
				opposingReadyExist = true;
				break;
			}
		}

		if (opposingReadyExist)
			result = HostGameStartableResult::Startable;
		else
			result = HostGameStartableResult::NotReady;
	}
		break;
	case GameRoomState::InPlay:
		result = HostGameStartableResult::AlreadyStarted;
		break;
	default:
		result = HostGameStartableResult::NotReady;
		break;
	}

	return result;
}

void GameRoom::ChangeHostAndReadyPlayersStateToPlaying(winppy::TCPServer& server)
{
	this->ChangePlayerState(server, m_pHost->GetAccountId(), PlayerState::Playing);

	for (auto& item : m_redTeam)
	{
		if (item.m_pPlayer == m_pHost || item.m_state == PlayerState::Ready)
		{
			item.m_state = PlayerState::Playing;

			SCNotifyPlayerStateChanged psc;
			psc.m_accountId = item.m_pPlayer->GetAccountId();
			psc.m_newState = item.m_state;

			winppy::Packet pkt;
			pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_STATE_CHANGED));
			pkt->WriteBytes(&psc, sizeof(psc));

			BroadcastPacket(server, std::move(pkt));
		}
	}

	for (auto& item : m_blueTeam)
	{
		if (item.m_pPlayer == m_pHost || item.m_state == PlayerState::Ready)
		{
			item.m_state = PlayerState::Playing;

			SCNotifyPlayerStateChanged psc;
			psc.m_accountId = item.m_pPlayer->GetAccountId();
			psc.m_newState = item.m_state;

			winppy::Packet pkt;
			pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_STATE_CHANGED));
			pkt->WriteBytes(&psc, sizeof(psc));

			BroadcastPacket(server, std::move(pkt));
		}
	}
}

void GameRoom::ChangePlayerState(winppy::TCPServer& server, uint32_t accountId, PlayerState newState)
{
	GameTeam team;
	size_t index;
	PlayerState oldState;

	if (!FindPlayer(accountId, team, index, oldState))
		*reinterpret_cast<int*>(0) = 0;

	switch (team)
	{
	case GameTeam::RedTeam:
		m_redTeam[index].m_state = newState;
		break;
	case GameTeam::BlueTeam:
		m_blueTeam[index].m_state = newState;
		break;
	default:
		*reinterpret_cast<int*>(0) = 0;
		break;
	}

	// 방에 있는 플레이어들에게 팀 변경된 플레이어 통지
	SCNotifyPlayerStateChanged psc;
	psc.m_accountId = accountId;
	psc.m_newState = newState;

	winppy::Packet pkt;
	pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_STATE_CHANGED));
	pkt->WriteBytes(&psc, sizeof(psc));

	this->BroadcastPacket(server, std::move(pkt));
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

bool GameRoom::ChangePlayerTeam(winppy::TCPServer& server, uint32_t accountId, GameTeam newTeam)
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
		player = m_redTeam[index];		// PlayerState 포함 정보 복사 (중요!)
		m_redTeam.erase(m_redTeam.begin() + index);
		break;
	case GameTeam::BlueTeam:
		player = m_blueTeam[index];		// PlayerState 포함 정보 복사 (중요!)
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

	SCNotifyPlayerTeamChanged noti;
	noti.m_accountId = accountId;
	noti.m_newTeam = newTeam;

	winppy::Packet pkt;
	pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_TEAM_CHANGED));
	pkt->WriteBytes(&noti, sizeof(noti));

	this->BroadcastPacket(server, std::move(pkt));

	return true;
}

GameTeam GameRoom::GetPlayerTeam(uint32_t accountId) const
{
	GameTeam team;
	size_t index;
	PlayerState state;

	if (!this->FindPlayer(accountId, team, index, state))
		return GameTeam::Unknown;
	else
		return team;
}

bool GameRoom::IsFull() const
{
	if (m_redTeam.size() + m_blueTeam.size() >= GameRoomTeamFormatToMaxPlayerCount(m_tf))
		return true;
	else
		return false;
}

void GameRoom::BroadcastPacket(winppy::TCPServer& server, winppy::Packet packet) const
{
	for (const auto& item : m_redTeam)
		server.Send(item.m_pPlayer->GetSession()->GetNetId(), packet);

	for (const auto& item : m_blueTeam)
		server.Send(item.m_pPlayer->GetSession()->GetNetId(), packet);
}

void GameRoom::BroadcastPacketExcept(winppy::TCPServer& server, winppy::Packet packet, uint32_t exceptorAccountId) const
{
	for (const auto& item : m_redTeam)
	{
		if (item.m_pPlayer->GetAccountId() == exceptorAccountId)
			continue;

		server.Send(item.m_pPlayer->GetSession()->GetNetId(), packet);
	}

	for (const auto& item : m_blueTeam)
	{
		if (item.m_pPlayer->GetAccountId() == exceptorAccountId)
			continue;

		server.Send(item.m_pPlayer->GetSession()->GetNetId(), packet);
	}
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

const Player* GameRoom::SelectRandomPlayer() const
{
	if (!m_redTeam.empty())
		return m_redTeam.front().m_pPlayer;

	if (!m_blueTeam.empty())
		return m_blueTeam.front().m_pPlayer;

	return nullptr;
}
