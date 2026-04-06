#include "ChJobReqJoinGameRoom.h"
#include "GameServer.h"
#include "GameSession.h"
#include "GameRoom.h"
#include "Protocol.h"
#include <cassert>

void ChJobReqJoinGameRoom::Execute(GameChannel& channel)
{
	CSReqJoinGameRoom req;

	if (!m_packet->ReadBytes(&req, sizeof(req)))
	{
		m_server.Disconnect(m_spSession->GetNetId());
		return;
	}

	if (m_spSession->GetJoiningGameRoom() != nullptr)
	{
		m_server.Disconnect(m_spSession->GetNetId());
		return;
	}

	const auto& gameRooms = channel.GameRooms();
	auto iter = gameRooms.find(req.m_gameRoomId);
	if (iter == gameRooms.cend())
	{
		SCResJoinGameRoom res;
		res.m_result = false;
		res.m_reason = FailReason::InvalidGame;

		winppy::Packet outPacket;
		outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_JOIN_GAME_ROOM));
		outPacket->WriteBytes(&res, sizeof(res));
		m_server.Send(m_spSession->GetNetId(), std::move(outPacket));
		return;
	}

	auto spGameRoom = iter->second;
	assert(spGameRoom->m_spHost);


	// 만약 방이 다 찬 경우
	const size_t maxPlayerPerTeam = static_cast<size_t>(spGameRoom->GetMaxPlayer()) + 1;
	if (spGameRoom->m_redTeamSessionsCount + spGameRoom->m_blueTeamSessionsCount == maxPlayerPerTeam * 2)
	{
		SCResJoinGameRoom res;
		res.m_result = false;
		res.m_reason = FailReason::Full;

		winppy::Packet outPacket;
		outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_JOIN_GAME_ROOM));
		outPacket->WriteBytes(&res, sizeof(res));
		m_server.Send(m_spSession->GetNetId(), std::move(outPacket));
		return;
	}

	// 아래부터 순서 다시 조정
	// 1. 세션을 방 자료구조에 넣고
	// 2. 입장을 요청한 세션에게 결과 패킷 전송
	// 2. 기존 방 플레이어들에겐 새로 입장하는 플레이어의 정보를
	// 3. 새로 입장하는 유저에겐 기존 방 플레이어들의 정보를 보내고

	
	// 빈 자리 찾아서 세션 삽입
	GameTeam joinedTeam = GameTeam::Unknown;
	if (spGameRoom->m_redTeamSessionsCount < maxPlayerPerTeam)
	{
		joinedTeam = GameTeam::RedTeam;
		spGameRoom->m_redTeamSessions[spGameRoom->m_redTeamSessionsCount] = m_spSession;
		++spGameRoom->m_redTeamSessionsCount;
	}
	else if (spGameRoom->m_blueTeamSessionsCount < maxPlayerPerTeam)
	{
		joinedTeam = GameTeam::BlueTeam;
		spGameRoom->m_blueTeamSessions[spGameRoom->m_blueTeamSessionsCount] = m_spSession;
		++spGameRoom->m_blueTeamSessionsCount;
	}
	m_spSession->SetJoiningGameRoom(spGameRoom);	// 설정...

	// 결과 패킷 전송...
	SCResJoinGameRoom res;
	res.m_result = true;
	res.m_reason = FailReason::Success;
	res.m_gameRoomId = spGameRoom->GetRoomId();
	res.m_gameRoomNo = spGameRoom->GetRoomNo();
	res.m_gameRoomHostNetId = spGameRoom->m_spHost->GetNetId();
	res.m_maxPlayer = spGameRoom->GetMaxPlayer();
	res.m_gameMap = spGameRoom->GetGameMap();
	res.m_gameMode = spGameRoom->GetGameMode();
	res.m_joinedTeam = joinedTeam;
	res.m_gameRoomNameLen = spGameRoom->GetNameLen();
	wmemcpy_s(res.m_gameRoomName, _countof(res.m_gameRoomName), spGameRoom->GetName(), spGameRoom->GetNameLen());

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_JOIN_GAME_ROOM));
	outPacket->WriteBytes(&res, sizeof(res));
	m_server.Send(m_spSession->GetNetId(), std::move(outPacket));


	
	SCNotifyPlayerJoined notifyPlayerJoined;
	notifyPlayerJoined.m_gameRoomId = spGameRoom->GetRoomId();
	notifyPlayerJoined.m_netId = m_spSession->GetNetId();
	notifyPlayerJoined.m_joinedTeam = joinedTeam;
	notifyPlayerJoined.m_level = m_spSession->GetLevel();
	notifyPlayerJoined.m_nicknameLen = m_spSession->GetNicknameLen();
	wmemcpy_s(notifyPlayerJoined.m_nickname, _countof(notifyPlayerJoined.m_nickname), m_spSession->GetNickname(), m_spSession->GetNicknameLen());

	winppy::Packet toAlreadyExistingPlayer;
	toAlreadyExistingPlayer->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_JOINED_GAME_ROOM));
	toAlreadyExistingPlayer->WriteBytes(&notifyPlayerJoined, sizeof(notifyPlayerJoined));	// 현재 입장하는 플레이어에 대한 정보 write

	for (uint8_t i = 0; i < spGameRoom->m_redTeamSessionsCount; ++i)
	{
		const GameSession& player = *spGameRoom->m_redTeamSessions[i].get();

		if (player.GetNetId() == m_spSession->GetNetId())
			continue;

		m_server.Send(player.GetNetId(), toAlreadyExistingPlayer);

		SCNotifyGameRoomPlayer notifyPlayer;
		notifyPlayer.m_gameRoomId = req.m_gameRoomId;
		notifyPlayer.m_netId = player.GetNetId();
		notifyPlayer.m_team = GameTeam::RedTeam;
		notifyPlayer.m_level = player.GetLevel();
		notifyPlayer.m_nicknameLen = player.GetNicknameLen();
		wmemcpy_s(notifyPlayer.m_nickname, _countof(notifyPlayer.m_nickname), player.GetNickname(), player.GetNicknameLen());

		winppy::Packet toJoiningPlayer;
		toJoiningPlayer->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_GAME_ROOM_PLAYER));
		toJoiningPlayer->WriteBytes(&notifyPlayer, sizeof(notifyPlayer));
		m_server.Send(m_spSession->GetNetId(), std::move(toJoiningPlayer));
	}
	for (uint8_t i = 0; i < spGameRoom->m_blueTeamSessionsCount; ++i)
	{
		const GameSession& player = *spGameRoom->m_blueTeamSessions[i].get();

		if (player.GetNetId() == m_spSession->GetNetId())
			continue;

		m_server.Send(player.GetNetId(), toAlreadyExistingPlayer);

		SCNotifyGameRoomPlayer notifyPlayer;
		notifyPlayer.m_gameRoomId = req.m_gameRoomId;
		notifyPlayer.m_netId = player.GetNetId();
		notifyPlayer.m_team = GameTeam::BlueTeam;
		notifyPlayer.m_level = player.GetLevel();
		notifyPlayer.m_nicknameLen = player.GetNicknameLen();
		wmemcpy_s(notifyPlayer.m_nickname, _countof(notifyPlayer.m_nickname), player.GetNickname(), player.GetNicknameLen());

		winppy::Packet toJoiningPlayer;
		toJoiningPlayer->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_GAME_ROOM_PLAYER));
		toJoiningPlayer->WriteBytes(&notifyPlayer, sizeof(notifyPlayer));
		m_server.Send(m_spSession->GetNetId(), std::move(toJoiningPlayer));
	}
}
