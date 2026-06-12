#include "LogicThread.h"
#include "Protocol.h"
#include <cassert>
#include <strsafe.h>

void PlayerExitRoom(LogicThread& thread, Player* pPlayer)
{
	assert(pPlayer->IsInRoom());
	assert(pPlayer->IsInChannel());

	if (pPlayer->IsInRoom())
	{
		const uint8_t joinedChannelId = pPlayer->GetChannelId();
		const uint64_t joinedRoomId = pPlayer->GetRoomId();

		Channel& joinedChannel = thread.m_channel[joinedChannelId];
		GameRoom* pGameRoom = joinedChannel.FindRoom(joinedRoomId);
		assert(pGameRoom != nullptr);

		RemovePlayerResult removeResult = pGameRoom->RemovePlayer(pPlayer);

		// ######################################################
		// 나간 플레이어에게 전달되는 패킷
		SCResExitGameRoom res;
		res.m_result = true;
		winppy::Packet pktResExitGameRoom;
		pktResExitGameRoom->Write(static_cast<protocol_type>(Protocol::SC_RES_EXIT_GAME_ROOM));
		pktResExitGameRoom->WriteBytes(&res, sizeof(res));
		thread.m_server.Send(pPlayer->GetSession()->GetNetId(), pktResExitGameRoom);
		// ######################################################


		// ######################################################
		// 나간 플레이어를 제외한 나머지 플레이어들에게 전달되는 패킷
		SCNotifyPlayerExitGameRoom notifyPlayerExitRoom;
		notifyPlayerExitRoom.m_accountId = pPlayer->GetAccountId();
		winppy::Packet pktNotifyPlayerExitRoom;
		pktNotifyPlayerExitRoom->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_EXIT_GAME_ROOM));
		pktNotifyPlayerExitRoom->WriteBytes(&notifyPlayerExitRoom, sizeof(notifyPlayerExitRoom));

		switch (removeResult)
		{
		case RemovePlayerResult::Normal:
			pGameRoom->BroadcastPacket(thread.m_server, pktNotifyPlayerExitRoom);
			break;
		case RemovePlayerResult::HostChanged:
			pGameRoom->BroadcastPacket(thread.m_server, pktNotifyPlayerExitRoom);
			{
				SCNotifyHostChanged notifyHostChanged;
				notifyHostChanged.m_newHostAccountId = pGameRoom->GetHost()->GetAccountId();
				winppy::Packet pktNotifyHostChanged;
				pktNotifyHostChanged->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_HOST_CHANGED));
				pktNotifyHostChanged->WriteBytes(&notifyHostChanged, sizeof(notifyHostChanged));
				pGameRoom->BroadcastPacket(thread.m_server, pktNotifyHostChanged);
			}
			break;
		case RemovePlayerResult::LastPlayerRemoved:
			joinedChannel.RemoveGameRoom(joinedRoomId);	// 이 이후로 roomIter & pGameRoom은 유효하지 않음.
			break;
		default:
			break;
		}
		// ######################################################
	}
}

void PlayerExitChannel(LogicThread& thread, Player* pPlayer)
{
	assert(pPlayer->IsInChannel());

	const uint8_t joinedChannelId = pPlayer->GetChannelId();
	Channel& channel = thread.m_channel[joinedChannelId];
	channel.RemovePlayer(pPlayer);

	// 패킷 전송
	SCResExitGameChannel res;
	res.m_result = true;

	winppy::Packet pktResJoinChannel;
	pktResJoinChannel->Write(static_cast<protocol_type>(Protocol::SC_RES_EXIT_GAME_CHANNEL));
	pktResJoinChannel->WriteBytes(&res, sizeof(res));

	thread.m_server.Send(pPlayer->GetSession()->GetNetId(), std::move(pktResJoinChannel));
}

void JobCreateNewSession::Execute(LogicThread& thread)
{
	thread.m_sessions.insert(std::make_pair(m_netId, std::make_unique<Session>(m_netId)));
}

void JobSessionDisconnected::Execute(LogicThread& thread)
{
	auto iter = thread.m_sessions.find(m_netId);
	if (iter == thread.m_sessions.end())	// 비정상적인 상황
	{
		*reinterpret_cast<int*>(0) = 0;
		return;
	}

	Session* pSession = iter->second.get();
	Player* pPlayer = pSession->GetPlayer();

	// 로
	if (pPlayer)
	{
		// 1. 만약 플레이어가 방에 입장해있는 상태인 경우
		if (pPlayer->IsInRoom())
			PlayerExitRoom(thread, pPlayer);

		// 2. 만약 플레이어가 채널에 입장해있는 상태인 경우
		if (pPlayer->IsInChannel())
			PlayerExitChannel(thread, pPlayer);

		pSession->UnbindPlayer();
		pPlayer->UnbindSession();
		thread.m_players.erase(pPlayer->GetAccountId());	// Player 객체 제거
		pPlayer = nullptr;	// 댕글링 포인터 무효화
	}
	
	thread.m_sessions.erase(m_netId);	// Session 객체 제거
	pSession = nullptr;
}

JobReqLogin::JobReqLogin(uint64_t netId, const wchar_t* id, const wchar_t* pw)
	: m_netId(netId)
{
	StringCchCopyW(m_id, _countof(m_id), id);
	StringCchCopyW(m_pw, _countof(m_pw), pw);
}

void JobReqLogin::Execute(LogicThread& thread)
{
	wprintf(L"Login Request - id: %s, pw: %s\n", m_id, m_pw);


	// 테스트 코드
	// 원래는 DB 스레드에 조회 작업 넘기고 DB스레드에서 다시 LogicThread에게 로그인인증결과처리작업을 디스패칭해야함.

	const uint32_t TEST_ACCOUNT_ID = ((rand() << 16) & 0x11110000) | rand();
	const wchar_t* TEST_NICKNAME = L"치즈피자";
	SCResLogin res;
	res.m_result = true;
	res.m_accountId = TEST_ACCOUNT_ID;
	res.m_nicknameLen = wcslen(TEST_NICKNAME);
	wmemcpy_s(res.m_nickname, _countof(res.m_nickname), TEST_NICKNAME, res.m_nicknameLen);
	res.m_level = 8;
	res.m_exp = 2200;
	res.m_point = 5000;

	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	Session* pSession = sessionIter->second.get();

	std::unique_ptr<Player> upPlayer = std::make_unique<Player>(TEST_ACCOUNT_ID, res.m_level, res.m_point, TEST_NICKNAME);
	upPlayer->BindSession(pSession);
	pSession->BindPlayer(upPlayer.get());
	thread.m_players.insert(std::make_pair(upPlayer->GetAccountId(), std::move(upPlayer)));

	winppy::Packet pktResLogin;
	pktResLogin->Write(static_cast<protocol_type>(Protocol::SC_RES_LOGIN));
	pktResLogin->WriteBytes(&res, sizeof(res));
	thread.m_server.Send(m_netId, std::move(pktResLogin));
}

JobReqIdDuplicateCheck::JobReqIdDuplicateCheck(uint64_t netId, const wchar_t* id)
	: m_netId(netId)
{
	wcscpy_s(m_id, id);
}

void JobReqIdDuplicateCheck::Execute(LogicThread& thread)
{
	// 
}

JobReqNicknameDuplicateCheck::JobReqNicknameDuplicateCheck(uint64_t netId, const wchar_t* nickname)
	: m_netId(netId)
{
	wcscpy_s(m_nickname, nickname);
}

void JobReqNicknameDuplicateCheck::Execute(LogicThread& thread)
{
	// 
}

void JobReqChannelInfo::Execute(LogicThread& thread)
{
	// 패킷 전송
	SCResChannelInfo res;
	res.m_worldId = 0;
	for (size_t channelId = 0; channelId < thread.m_channel.size(); ++channelId)
	{
		res.m_channelInfo[channelId].m_numOfPlayers = static_cast<uint16_t>(thread.m_channel[channelId].GetNumOfPlayers());
		res.m_channelInfo[channelId].m_maxPlayerCount = MAX_PLAYERS_PER_CHANNEL;
	}


	winppy::Packet pktChannelInfo;
	pktChannelInfo->Write(static_cast<protocol_type>(Protocol::SC_RES_CHANNEL_INFO));
	pktChannelInfo->WriteBytes(&res, sizeof(res));

	thread.m_server.Send(m_netId, std::move(pktChannelInfo));
}

void JobReqJoinChannel::Execute(LogicThread& thread)
{
	assert(m_channelId < thread.m_channel.size());

	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	const Session* pSession = sessionIter->second.get();
	Player* pPlayer = pSession->GetPlayer();
	if (!pPlayer)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	Channel& channel = thread.m_channel[m_channelId];
	const bool result = channel.AddPlayer(pPlayer);

	// 패킷 전송
	SCResJoinChannel res;
	res.m_result = result;

	winppy::Packet pktResJoinChannel;
	pktResJoinChannel->Write(static_cast<protocol_type>(Protocol::SC_RES_JOIN_CHANNEL));
	pktResJoinChannel->WriteBytes(&res, sizeof(res));

	thread.m_server.Send(m_netId, std::move(pktResJoinChannel));
}

LogicThread::LogicThread(winppy::TCPServer& server)
	: m_server(server)
	, m_sessions()
	, m_players()
	, m_channel()
{
	m_channel.reserve(CHANNEL_COUNT);

	for (uint8_t channelId = 0; channelId < CHANNEL_COUNT; ++channelId)
		m_channel.emplace_back(channelId, MAX_PLAYERS_PER_CHANNEL);
}

void JobReqExitChannel::Execute(LogicThread& thread)
{
	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	const Session* pSession = sessionIter->second.get();
	Player* pPlayer = pSession->GetPlayer();
	if (!pPlayer)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	// 방에 입장해있는 상태이면 먼저 방에서 퇴장 처리
	if (pPlayer->IsInRoom())
	{
		PlayerExitRoom(thread, pPlayer);
	}

	// 채널에 입장해있지 않은데 이 패킷을 보낸 경우는 예외 처리
	if (!pPlayer->IsInChannel())
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}
	PlayerExitChannel(thread, pPlayer);
}

void JobAuthSession::Execute(LogicThread& thread)
{
	// 인증은 DB스레드에 의해서 비동기로 진행되기 때문에 현재 시점에서는 세션이 다시 나갔을 수 있다.
	// 이외에 하나 더 주의해야 할 점은 netId가 재사용된 경우, 실제로는 다른 사람이 해당 계정 정보를 가지고 플레이를 이어나갈 수도 있다.
	// 물론 세션 아이디의 재사용 주기는 상당히 길고 (재사용 32비트, 슬롯 인덱스 32비트)이므로 가능성이 0은 아니다.

	/*
	if (thread.m_sessions.find(m_netId))
	{

	}
	*/
}

void JobReqLobbyChat::Execute(LogicThread& thread)
{
	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	const Session* pSession = sessionIter->second.get();
	Player* pPlayer = pSession->GetPlayer();
	if (!pPlayer)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	if (!pPlayer->IsInChannel())
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	SCNotifyLobbyChat notifyLobbyChat;
	notifyLobbyChat.m_accountId = pPlayer->GetAccountId();
	notifyLobbyChat.m_nicknameLen = pPlayer->GetNicknameLen();
	notifyLobbyChat.m_msgLen = m_msgLen;
	winppy::Packet pktNotifyLobbyChat;
	pktNotifyLobbyChat->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_LOBBY_CHAT));
	pktNotifyLobbyChat->WriteBytes(&notifyLobbyChat, sizeof(notifyLobbyChat));
	pktNotifyLobbyChat->WriteBytes(pPlayer->GetNickname(), sizeof(wchar_t) * notifyLobbyChat.m_nicknameLen);
	pktNotifyLobbyChat->WriteBytes(m_msg, sizeof(wchar_t) * m_msgLen);

	if (pPlayer->IsInRoom())
	{
		const uint8_t joinedChannelId = pPlayer->GetChannelId();
		const uint64_t joinedRoomId = pPlayer->GetRoomId();
		const Channel& joinedChannel = thread.m_channel[joinedChannelId];

		const GameRoom* pGameRoom = joinedChannel.FindRoom(joinedRoomId);
		assert(pGameRoom != nullptr);

		pGameRoom->BroadcastPacket(thread.m_server, std::move(pktNotifyLobbyChat));
	}
	else
	{
		const uint8_t joinedChannelId = pPlayer->GetChannelId();
		assert(joinedChannelId < CHANNEL_COUNT);

		thread.m_channel[joinedChannelId].BroadcastPacketToPlayersNotInRoom(thread.m_server, std::move(pktNotifyLobbyChat));
	}
}

JobReqLobbyChat::JobReqLobbyChat(uint64_t netId, uint16_t msgLen, const wchar_t* msg)
	: m_netId(netId)
	, m_msgLen(msgLen)
{
	assert(m_msgLen <= MAX_CHAT_MSG_LEN);
	wmemcpy(m_msg, msg, m_msgLen);
	m_msg[m_msgLen] = L'\0';
}

JobReqCreateGameRoom::JobReqCreateGameRoom(uint64_t netId, GameRoomTeamFormat tf, uint16_t roomNameLen, wchar_t* roomName)
	: m_netId(netId)
	, m_roomTeamFormat(tf)
	, m_roomNameLen(roomNameLen)
{
	assert(m_roomNameLen <= MAX_GAME_ROOM_NAME_LEN);
	wmemcpy(m_roomName, roomName, m_roomNameLen);
	m_roomName[m_roomNameLen] = L'\0';
}

void JobReqCreateGameRoom::Execute(LogicThread& thread)
{
	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	const Session* pSession = sessionIter->second.get();
	Player* pPlayer = pSession->GetPlayer();
	if (!pPlayer)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	if (!pPlayer->IsInChannel() || pPlayer->IsInRoom())
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	const uint8_t joinedChannelId = pPlayer->GetChannelId();
	Channel& joinedChannel = thread.m_channel[joinedChannelId];
	GameRoom* pNewGameRoom = joinedChannel.CreateGameRoom(m_roomTeamFormat, m_roomName);

	SCResCreateGameRoom res;
	if (!pNewGameRoom)
	{
		res.m_result = false;
	}
	else
	{
		GameTeam joinedTeam;
		bool b = pNewGameRoom->AddPlayerAsHost(pPlayer, joinedTeam);
		if (!b)
			*reinterpret_cast<int*>(0) = 0;

		const GameRoomTeamFormat gameRoomTeamFormat = pNewGameRoom->GetTeamFormat();
		const GameMap gameRoomMap = pNewGameRoom->GetMap();

		res.m_result = true;
		res.m_gameRoomId = pNewGameRoom->GetId();
		res.m_gameRoomNo = pNewGameRoom->GetNo();
		res.m_gameRoomHostAccountId = pNewGameRoom->GetHost()->GetAccountId();
		res.m_gameRoomTeamFormat = pNewGameRoom->GetTeamFormat();
		res.m_gameMap = pNewGameRoom->GetMap();
		res.m_joinedTeam = joinedTeam;
		res.m_gameRoomNameLen = static_cast<uint16_t>(pNewGameRoom->GetName().length());
		wmemcpy_s(res.m_gameRoomName, _countof(res.m_gameRoomName), pNewGameRoom->GetName().c_str(), pNewGameRoom->GetName().length());
	}
	
	winppy::Packet pktResCreateGameRoom;
	pktResCreateGameRoom->Write(static_cast<protocol_type>(Protocol::SC_RES_CREATE_GAME_ROOM));
	pktResCreateGameRoom->WriteBytes(&res, sizeof(res));
	
	thread.m_server.Send(m_netId, std::move(pktResCreateGameRoom));
}

void JobReqGameRoomList::Execute(LogicThread& thread)
{
	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	const Session* pSession = sessionIter->second.get();
	Player* pPlayer = pSession->GetPlayer();
	if (!pPlayer)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	if (!pPlayer->IsInChannel())
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	const uint8_t joinedChannelId = pPlayer->GetChannelId();
	const Channel& joinedChannel = thread.m_channel[joinedChannelId];
	auto pkts = joinedChannel.CreateGameRoomListPackets(m_reqContextNo);

	for (size_t i = 0; i < pkts.size(); ++i)
		thread.m_server.Send(m_netId, std::move(pkts[i]));
}

void JobReqJoinGameRoom::Execute(LogicThread& thread)
{
	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	const Session* pSession = sessionIter->second.get();
	Player* pPlayer = pSession->GetPlayer();
	if (!pPlayer)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	if (!pPlayer->IsInChannel())
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	const uint8_t joinedChannelId = pPlayer->GetChannelId();
	Channel& joinedChannel = thread.m_channel[joinedChannelId];

	GameRoom* pGameRoom = joinedChannel.FindRoom(m_gameRoomId);

	bool joinedRoom = false;

	GameTeam joinedTeam = GameTeam::Unknown;
	SCResJoinGameRoom res;
	do
	{
		if (!pGameRoom)
		{
			res.m_result = JoinGameRoomResult::InvalidGame;
			break;
		}

		
		if (!pGameRoom->AddPlayer(pPlayer, joinedTeam))
		{
			res.m_result = JoinGameRoomResult::Full;
			break;
		}

		joinedRoom = true;

		res.m_result = JoinGameRoomResult::Success;
		res.m_gameRoomId = pGameRoom->GetId();
		res.m_gameRoomNo = pGameRoom->GetNo();
		res.m_gameRoomHostAccountId = pGameRoom->GetHost()->GetAccountId();
		res.m_gameRoomTeamFormat = pGameRoom->GetTeamFormat();
		res.m_gameMap = pGameRoom->GetMap();
		res.m_joinedTeam = joinedTeam;
		res.m_gameRoomNameLen = static_cast<uint16_t>(pGameRoom->GetName().length());
		wmemcpy_s(res.m_gameRoomName, _countof(res.m_gameRoomName), pGameRoom->GetName().c_str(), res.m_gameRoomNameLen);
	} while (false);

	winppy::Packet pktResJoinGameRoom;
	pktResJoinGameRoom->Write(static_cast<protocol_type>(Protocol::SC_RES_JOIN_GAME_ROOM));
	pktResJoinGameRoom->WriteBytes(&res, sizeof(res));
	thread.m_server.Send(m_netId, std::move(pktResJoinGameRoom));
	
	if (!joinedRoom)
		return;


	assert(joinedTeam != GameTeam::Unknown);

	// 먼저 들어가있던 사람들에게 전송할 패킷
	winppy::Packet pktNotifyNewPlayer = pGameRoom->CreateGameRoomPlayerJoinedPacket(pPlayer);
	pGameRoom->BroadcastPacketExceptPlayer(thread.m_server, std::move(pktNotifyNewPlayer), pPlayer);

	// 지금 들어간 플레이어에게 방에 존재하던 플레이어들 정보 전송
	auto pktsGameRoomPlayers = pGameRoom->CreateGameRoomPlayerPacketsExcept(pPlayer);
	for (size_t i = 0; i < pktsGameRoomPlayers.size(); ++i)
		thread.m_server.Send(m_netId, std::move(pktsGameRoomPlayers[i]));
}

void JobReqExitGameRoom::Execute(LogicThread& thread)
{
	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	const Session* pSession = sessionIter->second.get();
	Player* pPlayer = pSession->GetPlayer();
	if (!pPlayer)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	if (!pPlayer->IsInChannel() || !pPlayer->IsInRoom())
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	PlayerExitRoom(thread, pPlayer);
}

void JobReqChangeGameReadyState::Execute(LogicThread& thread)
{
	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	const Session* pSession = sessionIter->second.get();
	Player* pPlayer = pSession->GetPlayer();
	if (!pPlayer)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	if (!pPlayer->IsInChannel() || !pPlayer->IsInRoom())
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	const uint64_t joinedRoomId = pPlayer->GetRoomId();
	const uint8_t joinedChannelId = pPlayer->GetChannelId();
	Channel& joinedChannel = thread.m_channel[joinedChannelId];

	GameRoom* pGameRoom = joinedChannel.FindRoom(joinedRoomId);

	bool ret = pGameRoom->ChangePlayerState(pPlayer->GetAccountId(), m_ready ? PlayerState::Ready : PlayerState::None);
	if (!ret)
		*reinterpret_cast<int*>(0) = 0;

	winppy::Packet pktNotifyReadyStateChanged;
	if (m_ready)
	{
		SCNotifyPlayerGameReady notify;
		notify.m_accountId = pPlayer->GetAccountId();

		pktNotifyReadyStateChanged->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_GAME_READY));
		pktNotifyReadyStateChanged->WriteBytes(&notify, sizeof(notify));
	}
	else
	{
		SCNotifyPlayerGameUnready notify;
		notify.m_accountId = pPlayer->GetAccountId();

		pktNotifyReadyStateChanged->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_GAME_UNREADY));
		pktNotifyReadyStateChanged->WriteBytes(&notify, sizeof(notify));
	}

	pGameRoom->BroadcastPacket(thread.m_server, std::move(pktNotifyReadyStateChanged));
}

void JobReqChangeTeam::Execute(LogicThread& thread)
{
	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	const Session* pSession = sessionIter->second.get();
	Player* pPlayer = pSession->GetPlayer();
	if (!pPlayer)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	if (!pPlayer->IsInChannel() || !pPlayer->IsInRoom())
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	const uint64_t joinedRoomId = pPlayer->GetRoomId();
	const uint8_t joinedChannelId = pPlayer->GetChannelId();
	Channel& joinedChannel = thread.m_channel[joinedChannelId];

	GameRoom* pGameRoom = joinedChannel.FindRoom(joinedRoomId);
	if (pGameRoom->ChangePlayerTeam(pPlayer->GetAccountId(), m_newTeam))
	{
		SCNotifyPlayerTeamChanged noti;
		noti.m_accountId = pPlayer->GetAccountId();
		noti.m_newTeam = m_newTeam;

		winppy::Packet pktNotifyPlayerTeamChanged;
		pktNotifyPlayerTeamChanged->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_PLAYER_TEAM_CHANGED));
		pktNotifyPlayerTeamChanged->WriteBytes(&noti, sizeof(noti));

		pGameRoom->BroadcastPacket(thread.m_server, std::move(pktNotifyPlayerTeamChanged));
	}
	else
	{
		// 패킷 전송 X
	}
}
