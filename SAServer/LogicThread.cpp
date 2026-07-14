#include "LogicThread.h"
#include "Protocol.h"
#include "SAServer.h"
#include "Channel.h"
#include "DBThread.h"
#include <cassert>
#include <strsafe.h>

static bool IsAlpha(wchar_t ch)
{
	return (L'A' <= ch && ch <= L'Z') || (L'a' <= ch && ch <= L'z');
}

static bool IsNum(wchar_t ch)
{
	return L'0' <= ch && ch <= '9';
}

static bool IsSpecialChar(wchar_t ch)
{
	switch (ch)
	{
	case L'!':
	case L'@':
	case L'#':
	case L'$':
	case L'%':
	case L'^':
	case L'&':
	case L'*':
	case L'(':
	case L')':
	case L'-':
	case L'_':
	case L'=':
	case L'+':
		return true;
	default:
		return false;
	}
}

static bool IsAlphaNumOnly(const wchar_t* str)
{
	if (str == nullptr)
		return false;

	while (*str != L'\0')
	{
		const wchar_t ch = *str;

		if (!IsAlpha(ch) && !IsNum(ch))
			return false;

		++str;
	}

	return true;
}

static bool IsAlphaNumSpecialOnly(const wchar_t* str)
{
	if (str == nullptr)
		return false;

	while (*str != L'\0')
	{
		const wchar_t ch = *str;

		if (!IsAlpha(ch) && !IsNum(ch) && !IsSpecialChar(ch))
			return false;

		++str;
	}

	return true;
}

void PlayerExitRoom(LogicThread& thread, Player* pPlayer)
{
	assert(pPlayer->IsInRoom());
	assert(pPlayer->IsInChannel());
	assert(pPlayer->IsInChannel());

	const uint8_t joinedChannelId = pPlayer->GetChannelId();
	const uint64_t joinedRoomId = pPlayer->GetRoomId();

	Channel& joinedChannel = *thread.m_channel[joinedChannelId];
	GameRoom* pGameRoom = joinedChannel.FindRoom(joinedRoomId);
	assert(pGameRoom != nullptr);

	const size_t numOfRemainingPlayers = pGameRoom->RemovePlayer(thread.m_server, pPlayer);
	if (numOfRemainingPlayers == 0)
		joinedChannel.RemoveGameRoom(joinedRoomId);				// 이 이후로 roomIter & pGameRoom은 유효하지 않음
}

void PlayerExitChannel(LogicThread& thread, Player* pPlayer)
{
	assert(pPlayer->IsInChannel());

	const uint8_t joinedChannelId = pPlayer->GetChannelId();
	Channel& channel = *thread.m_channel[joinedChannelId];
	channel.RemovePlayer(thread.m_server, pPlayer);
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

	// 
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
	wprintf(L"Login request - id: %s, pw: %s\n", m_id, m_pw);

	if (!IsAlphaNumOnly(m_id) || !IsAlphaNumSpecialOnly(m_pw))	// 아이디 또는 비밀번호에 유효하지 않은 문자가 있는 경우 (클라이언트에서는 막아놓음)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	Session* pSession = sessionIter->second.get();

	// DB 스레드에 작업 위임
	std::unique_ptr<DBJobLogin> upDBJob = std::make_unique<DBJobLogin>(m_netId, m_id, m_pw);
	assert(pSession->GetDBThreadIndex() < DB_THREAD_COUNT);
	thread.m_server.GetDBThread(pSession->GetDBThreadIndex()).DispatchJob(std::move(upDBJob));
}

JobReqIdDuplicateCheck::JobReqIdDuplicateCheck(uint64_t netId, const wchar_t* id)
	: m_netId(netId)
{
	StringCchCopyW(m_id, _countof(m_id), id);
}

void JobReqIdDuplicateCheck::Execute(LogicThread& thread)
{
	wprintf(L"ID duplicate check request - id: %s, \n", m_id);

	if (!IsAlphaNumOnly(m_id))		// 아이디에 유효하지 않은 문자가 있는 경우 (클라이언트에서는 막아놓음)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	Session* pSession = sessionIter->second.get();

	// DB 스레드에 작업 위임
	std::unique_ptr<DBJobIdDuplicateCheck> upDBJob = std::make_unique<DBJobIdDuplicateCheck>(m_netId, m_id);
	assert(pSession->GetDBThreadIndex() < DB_THREAD_COUNT);
	thread.m_server.GetDBThread(pSession->GetDBThreadIndex()).DispatchJob(std::move(upDBJob));
}

JobReqNicknameDuplicateCheck::JobReqNicknameDuplicateCheck(uint64_t netId, const wchar_t* nickname)
	: m_netId(netId)
{
	StringCchCopyW(m_nickname, _countof(m_nickname), nickname);
}

void JobReqNicknameDuplicateCheck::Execute(LogicThread& thread)
{
	wprintf(L"Nickname duplicate check request - nickname: %s, \n", m_nickname);

	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	Session* pSession = sessionIter->second.get();

	// DB 스레드에 작업 위임
	std::unique_ptr<DBJobNicknameDuplicateCheck> upDBJob = std::make_unique<DBJobNicknameDuplicateCheck>(m_netId, m_nickname);
	assert(pSession->GetDBThreadIndex() < DB_THREAD_COUNT);
	thread.m_server.GetDBThread(pSession->GetDBThreadIndex()).DispatchJob(std::move(upDBJob));
}

void JobReqChannelInfo::Execute(LogicThread& thread)
{
	// 패킷 전송
	SCResChannelInfo res;
	res.m_worldId = 0;
	for (size_t channelId = 0; channelId < thread.m_channel.size(); ++channelId)
	{
		res.m_channelInfo[channelId].m_numOfPlayers = static_cast<uint16_t>(thread.m_channel[channelId]->GetNumOfPlayers());
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

	Channel& channel = *thread.m_channel[m_channelId];
	channel.AddPlayer(thread.m_server, pPlayer);
}

LogicThread::LogicThread(SAServer& server)
	: m_server(server)
	, m_sessions()
	, m_players()
	, m_channel()
{
	m_channel.reserve(CHANNEL_COUNT);

	for (uint8_t channelId = 0; channelId < CHANNEL_COUNT; ++channelId)
		m_channel.push_back(std::make_unique<Channel>(channelId, MAX_PLAYERS_PER_CHANNEL));
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
	winppy::Packet pkt;
	pkt->Write(static_cast<protocol_type>(Protocol::SC_NOTIFY_LOBBY_CHAT));
	pkt->WriteBytes(&notifyLobbyChat, sizeof(notifyLobbyChat));
	pkt->WriteBytes(pPlayer->GetNickname(), sizeof(wchar_t) * notifyLobbyChat.m_nicknameLen);
	pkt->WriteBytes(m_msg, sizeof(wchar_t) * m_msgLen);

	if (pPlayer->IsInRoom())
	{
		const uint8_t joinedChannelId = pPlayer->GetChannelId();
		const uint64_t joinedRoomId = pPlayer->GetRoomId();
		const Channel& joinedChannel = *thread.m_channel[joinedChannelId];

		const GameRoom* pGameRoom = joinedChannel.FindRoom(joinedRoomId);
		assert(pGameRoom != nullptr);

		pGameRoom->BroadcastPacket(thread.m_server, std::move(pkt));
	}
	else
	{
		const uint8_t joinedChannelId = pPlayer->GetChannelId();
		assert(joinedChannelId < CHANNEL_COUNT);

		thread.m_channel[joinedChannelId]->BroadcastPacketExceptInRoomPlayers(thread.m_server, std::move(pkt));
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
	Channel& joinedChannel = *thread.m_channel[joinedChannelId];
	if (!joinedChannel.CreateGameRoom(thread.m_server, m_roomTeamFormat, m_roomName, pPlayer))
	{
		SCResCreateGameRoom res;
		res.m_result = false;

		winppy::Packet pkt;
		pkt->Write(static_cast<protocol_type>(Protocol::SC_RES_CREATE_GAME_ROOM));
		pkt->WriteBytes(&res, sizeof(res));

		thread.m_server.Send(m_netId, std::move(pkt));
	}
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
	const Channel& joinedChannel = *thread.m_channel[joinedChannelId];
	joinedChannel.SendGameRoomLists(thread.m_server, pPlayer, m_reqContextNo);
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
	Channel& joinedChannel = *thread.m_channel[joinedChannelId];
	GameRoom* pGameRoom = joinedChannel.FindRoom(m_gameRoomId);
	

	if (!pGameRoom)		// 이미 존재하지 않는 방인 경우
	{
		SCResJoinGameRoom res;
		res.m_result = JoinGameRoomResult::InvalidGame;

		winppy::Packet pkt;
		pkt->Write(static_cast<protocol_type>(Protocol::SC_RES_JOIN_GAME_ROOM));
		pkt->WriteBytes(&res, sizeof(res));
		thread.m_server.Send(pPlayer->GetSession()->GetNetId(), std::move(pkt));
		return;
	}

	if (pGameRoom->IsFull())		// 방이 다 찬 경우
	{
		SCResJoinGameRoom res;
		res.m_result = JoinGameRoomResult::Full;

		winppy::Packet pkt;
		pkt->Write(static_cast<protocol_type>(Protocol::SC_RES_JOIN_GAME_ROOM));
		pkt->WriteBytes(&res, sizeof(res));
		thread.m_server.Send(pPlayer->GetSession()->GetNetId(), std::move(pkt));

		return;
	}

	pGameRoom->AddPlayer(thread.m_server, pPlayer);
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

	// Playing 상태(게임에 진입중인 상태 & 진입 완료한 상태 등)이면 나가기 불허.
	const uint64_t joinedRoomId = pPlayer->GetRoomId();
	const uint8_t joinedChannelId = pPlayer->GetChannelId();
	Channel& joinedChannel = *thread.m_channel[joinedChannelId];
	GameRoom* pGameRoom = joinedChannel.FindRoom(joinedRoomId);
	if (pGameRoom->GetPlayerState(pPlayer->GetAccountId()) == PlayerState::Playing)
		return;

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
	Channel& joinedChannel = *thread.m_channel[joinedChannelId];
	GameRoom* pGameRoom = joinedChannel.FindRoom(joinedRoomId);

	pGameRoom->ChangePlayerState(thread.m_server, pPlayer->GetAccountId(), m_ready ? PlayerState::Ready : PlayerState::None);
}

void JobReqGameStartableState::Execute(LogicThread& thread)
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
	Channel& joinedChannel = *thread.m_channel[joinedChannelId];
	
	GameRoom* pGameRoom = joinedChannel.FindRoom(joinedRoomId);
	if (pGameRoom->GetHost() != pPlayer)		// 방장(게임시작 권한자)이 맞는지 확인
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	HostGameStartableResult ret = pGameRoom->IsGameStartable();	// 방장의 상대팀에 한 명 이상의 플레이어가 레디 상태인지 검사하는 함수 호출
	assert(ret != HostGameStartableResult::Unknown);
	switch (ret)
	{
	case HostGameStartableResult::AlreadyStarted:
		// 무시 (아무 작업도 하지 않음) (게임시작 버튼 빠르게 여러번 클릭 시 가능한 상황)
		break;
	case HostGameStartableResult::Startable:
	{
		pGameRoom->ChangeReadyPlayersAndHostStateToPlaying(thread.m_server);	// 준비 상태 플레이어 & 방장의 퇴장 불허
		pGameRoom->SetState(GameRoomState::InPlay);				// 게임 상태 '진행중'으로 변경

		// 1. 방장에게는 SC_RES_HOST_GAME_START 패킷 전송
		SCResHostGameStartableState res;
		res.m_result = HostGameStartableState::Startable;
		
		additional code...

		// 이후 방장으로부터 CS_NOTIFY_HOST_CREATED 패킷이 도착하면
		// 레디 상태에 있는 모든 플레이어들에게 방장의 enet 호스트 정보로 연결 시도하도록 패킷 브로드캐스트
	}
		break;
	case HostGameStartableResult::NotReady:
	{
		// 실패 패킷 전송
		SCResHostGameStartableState res;
		res.m_result = HostGameStartableState::NotReady;

		winppy::Packet pktResHostGameStart;
		pktResHostGameStart->Write(static_cast<protocol_type>(Protocol::SC_RES_HOST_GAME_STARTABLE_STATE));
		pktResHostGameStart->WriteBytes(&res, sizeof(res));

		thread.m_server.Send(pSession->GetNetId(), std::move(pktResHostGameStart));
	}
		break;
	default:
		break;
	}
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
	Channel& joinedChannel = *thread.m_channel[joinedChannelId];

	GameRoom* pGameRoom = joinedChannel.FindRoom(joinedRoomId);
	pGameRoom->ChangePlayerTeam(thread.m_server, pPlayer->GetAccountId(), m_newTeam);
}

JobReqCreateAccount::JobReqCreateAccount(uint64_t netId, const wchar_t* id, const wchar_t* nickname, const wchar_t* pw)
	: m_netId(netId)
{
	StringCchCopyW(m_id, _countof(m_id), id);
	StringCchCopyW(m_nickname, _countof(m_nickname), nickname);
	StringCchCopyW(m_pw, _countof(m_pw), pw);
}

void JobReqCreateAccount::Execute(LogicThread& thread)
{
	wprintf(L"Create account request - id: %s, nickname: %s, pw: %s\n", m_id, m_nickname, m_pw);

	if (!IsAlphaNumOnly(m_id) || !IsAlphaNumSpecialOnly(m_pw))	// 아이디 또는 비밀번호에 유효하지 않은 문자가 있는 경우 (클라이언트에서는 막아놓음)
	{
		thread.m_server.Disconnect(m_netId);
		return;
	}

	const auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())
		*reinterpret_cast<int*>(0) = 0;

	Session* pSession = sessionIter->second.get();

	// DB 스레드에 작업 위임
	std::unique_ptr<DBJobCreateAccount> upDBJob = std::make_unique<DBJobCreateAccount>(m_netId, m_id, m_nickname, m_pw);
	assert(pSession->GetDBThreadIndex() < DB_THREAD_COUNT);
	thread.m_server.GetDBThread(pSession->GetDBThreadIndex()).DispatchJob(std::move(upDBJob));
}

void JobDBJobReqLoginResult::Execute(LogicThread& thread)
{
	// 인증은 DB스레드에 의해서 비동기로 진행되기 때문에 현재 시점에서는 세션이 다시 나갔을 수 있다.
	// 이외에 하나 더 주의해야 할 점은 netId가 재사용된 경우, 실제로는 다른 사람이 해당 계정 정보를 가지고 플레이를 이어나갈 수도 있다.
	// 물론 세션 아이디의 재사용 주기는 상당히 길고 (재사용 32비트, 슬롯 인덱스 32비트)이므로 가능성이 0은 아니다.

	auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())				// 이미 세션이 나간 후인 경우 (DB 조회 작업은 비동기이므로 일어날 수 있는 일)
		return;

	Session* pSession = sessionIter->second.get();

	SCResLogin res;
	res.m_querySuccess = m_querySuccess;
	res.m_result = m_result;
	if (m_result)
	{
		res.m_accountId = m_accountId;
		res.m_nicknameLen = m_nicknameLen;
		wmemcpy_s(res.m_nickname, _countof(res.m_nickname), m_nickname, res.m_nicknameLen);
		res.m_level = m_level;
		res.m_exp = m_exp;
		res.m_point = m_point;

		std::unique_ptr<Player> upPlayer = std::make_unique<Player>(m_accountId, res.m_level, res.m_point, m_nickname);
		upPlayer->BindSession(pSession);
		pSession->BindPlayer(upPlayer.get());
		thread.m_players.insert(std::make_pair(upPlayer->GetAccountId(), std::move(upPlayer)));
	}

	winppy::Packet pktResLogin;
	pktResLogin->Write(static_cast<protocol_type>(Protocol::SC_RES_LOGIN));
	pktResLogin->WriteBytes(&res, sizeof(res));
	thread.m_server.Send(m_netId, std::move(pktResLogin));
}

void JobDBJobCreateAccountResult::Execute(LogicThread& thread)
{
	auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())				// 이미 세션이 나간 후인 경우
		return;

	Session* pSession = sessionIter->second.get();
	SCResCreateAccount res;
	res.m_result = m_result;

	winppy::Packet pktResCreateAccount;
	pktResCreateAccount->Write(static_cast<protocol_type>(Protocol::SC_RES_CREATE_ACCOUNT));
	pktResCreateAccount->WriteBytes(&res, sizeof(res));
	thread.m_server.Send(m_netId, std::move(pktResCreateAccount));
}

void JobDBJobIdDuplicateCheckResult::Execute(LogicThread& thread)
{
	auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())				// 이미 세션이 나간 후인 경우
		return;

	Session* pSession = sessionIter->second.get();

	SCResIdDuplicateCheck res;
	res.m_querySuccess = m_querySuccess;
	res.m_duplicated = m_duplicated;

	winppy::Packet pktResIdDuplicateCheck;
	pktResIdDuplicateCheck->Write(static_cast<protocol_type>(Protocol::SC_RES_ID_DUPLICATE_CHECK));
	pktResIdDuplicateCheck->WriteBytes(&res, sizeof(res));
	thread.m_server.Send(m_netId, std::move(pktResIdDuplicateCheck));
}

void JobDBJobNicknameDuplicateCheckResult::Execute(LogicThread& thread)
{
	auto sessionIter = thread.m_sessions.find(m_netId);
	if (sessionIter == thread.m_sessions.end())				// 이미 세션이 나간 후인 경우
		return;

	Session* pSession = sessionIter->second.get();

	SCResNicknameDuplicateCheck res;
	res.m_querySuccess = m_querySuccess;
	res.m_duplicated = m_duplicated;

	winppy::Packet pktResNicknameDuplicateCheck;
	pktResNicknameDuplicateCheck->Write(static_cast<protocol_type>(Protocol::SC_RES_NICKNAME_DUPLICATE_CHECK));
	pktResNicknameDuplicateCheck->WriteBytes(&res, sizeof(res));
	thread.m_server.Send(m_netId, std::move(pktResNicknameDuplicateCheck));
}

