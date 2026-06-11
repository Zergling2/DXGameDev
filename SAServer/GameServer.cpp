#include "GameServer.h"
#include "Protocol.h"

GameServer::GameServer()
	: m_logic(*this)
{
	for (size_t i = 0; i < _countof(m_dbThread); ++i)
		m_dbThread[i].Start();

	m_logic.Start();
}

bool GameServer::OnConnect(const wchar_t* ip, uint16_t port, uint64_t id)
{
	wprintf(L"[Connected] %s:%u. Session %llu.\n", ip, static_cast<uint32_t>(port), id);

	std::unique_ptr<JobCreateNewSession> upJob = std::make_unique<JobCreateNewSession>(id);
	m_logic.DispatchJob(std::move(upJob));

	return true;
}

void GameServer::OnReceive(uint64_t id, winppy::Packet packet)
{
	Protocol protocol;
	if (packet->ReadableSize() < sizeof(protocol))
	{
		Disconnect(id);
		return;
	}

	packet->Read(reinterpret_cast<protocol_type*>(&protocol));

	switch (protocol)
	{
	case Protocol::CS_REQ_LOGIN:
		OnCSReqLogin(id, std::move(packet));
		break;
	case Protocol::CS_REQ_ID_DUPLICATE_CHECK:
		OnCSReqIdDuplicateCheck(id, std::move(packet));
		break;
	case Protocol::CS_REQ_NICKNAME_DUPLICATE_CHECK:
		OnCSReqNicknameDuplicateCheck(id, std::move(packet));
		break;
	case Protocol::CS_REQ_CREATE_ACCOUNT:
		OnCSReqCreateAccount(id, std::move(packet));
		break;
	case Protocol::CS_REQ_CHANNEL_INFO:
		OnCSReqChannelInfo(id, std::move(packet));
		break;
	case Protocol::CS_REQ_JOIN_CHANNEL:
		OnCSReqJoinChannel(id, std::move(packet));
		break;
	case Protocol::CS_REQ_LOBBY_CHAT:
		OnCSReqLobbyChat(id, std::move(packet));
		break;
	case Protocol::CS_REQ_GAME_ROOM_LIST:
		OnCSReqGameRoomList(id, std::move(packet));
		break;
	case Protocol::CS_REQ_CREATE_GAME_ROOM:
		OnCSReqCreateGameRoom(id, std::move(packet));
		break;
	case Protocol::CS_REQ_JOIN_GAME_ROOM:
		OnCSReqJoinGameRoom(id, std::move(packet));
		break;
	case Protocol::CS_REQ_CHANGE_TEAM:
		OnCSReqChangeTeam(id, std::move(packet));
		break;
	case Protocol::CS_REQ_EXIT_GAME_ROOM:
		OnCSReqExitGameRoom(id, std::move(packet));
		break;
	case Protocol::CS_REQ_HOST_GAME_START:
		OnCSReqHostGameStart(id, std::move(packet));
		break;
	case Protocol::CS_REQ_GAME_READY:
		OnCSReqGameReady(id, std::move(packet));
		break;
	case Protocol::CS_REQ_GAME_UNREADY:
		OnCSReqGameUnready(id, std::move(packet));
		break;
	case Protocol::CS_REQ_EXIT_GAME_CHANNEL:
		OnCSReqExitGameChannel(id, std::move(packet));
		break;
	default:
		Disconnect(id);
		break;
	}
}

void GameServer::OnDisconnect(uint64_t id)
{
	wprintf(L"[Disconnected] Session %llu.\n", id);

	std::unique_ptr<JobSessionDisconnected> upJob = std::make_unique<JobSessionDisconnected>(id);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqLogin(uint64_t netId, winppy::Packet packet)
{
	CSReqLogin req;
	if (!packet->ReadBytes(&req, sizeof(req)))
	{
		Disconnect(netId);
		return;
	}

	if (req.m_idLen > MAX_ID_LEN || req.m_pwLen > MAX_PW_LEN)
	{
		Disconnect(netId);
		return;
	}

	wchar_t id[MAX_ID_LEN + 1];
	wmemcpy(id, req.m_id, req.m_idLen);
	id[req.m_idLen] = L'\0';

	wchar_t pw[MAX_PW_LEN + 1];
	wmemcpy(pw, req.m_pw, req.m_pwLen);
	pw[req.m_pwLen] = L'\0';

	std::unique_ptr<JobReqLogin> upJob = std::make_unique<JobReqLogin>(netId, id, pw);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqIdDuplicateCheck(uint64_t netId, winppy::Packet packet)
{
	CSReqIdDuplicateCheck req;
	if (!packet->ReadBytes(&req, sizeof(req)))
	{
		Disconnect(netId);
		return;
	}

	if (req.m_idLen > MAX_ID_LEN)
	{
		Disconnect(netId);
		return;
	}

	wchar_t id[MAX_ID_LEN + 1];
	wmemcpy(id, req.m_id, req.m_idLen);
	id[req.m_idLen] = L'\0';

	std::unique_ptr<JobReqIdDuplicateCheck> upJob = std::make_unique<JobReqIdDuplicateCheck>(netId, id);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqNicknameDuplicateCheck(uint64_t netId, winppy::Packet packet)
{
	CSReqNicknameDuplicateCheck req;
	if (!packet->ReadBytes(&req, sizeof(req)))
	{
		Disconnect(netId);
		return;
	}

	if (req.m_nicknameLen > MAX_NICKNAME_LEN)
	{
		Disconnect(netId);
		return;
	}

	wchar_t nickname[MAX_NICKNAME_LEN + 1];
	wmemcpy(nickname, req.m_nickname, req.m_nicknameLen);
	nickname[req.m_nicknameLen] = L'\0';

	std::unique_ptr<JobReqNicknameDuplicateCheck> upJob = std::make_unique<JobReqNicknameDuplicateCheck>(netId, nickname);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqCreateAccount(uint64_t netId, winppy::Packet packet)
{
	// 
}

void GameServer::OnCSReqChannelInfo(uint64_t netId, winppy::Packet packet)
{
	CSReqChannelInfo req;
	if (!packet->ReadBytes(&req, sizeof(req)))
	{
		Disconnect(netId);
		return;
	}

	std::unique_ptr<JobReqChannelInfo> upJob = std::make_unique<JobReqChannelInfo>(netId);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqJoinChannel(uint64_t netId, winppy::Packet packet)
{
	CSReqJoinChannel req;
	if (!packet->ReadBytes(&req, sizeof(req)))
	{
		Disconnect(netId);
		return;
	}

	if (req.m_channelId >= CHANNEL_COUNT)
	{
		Disconnect(netId);
		return;
	}

	std::unique_ptr<JobReqJoinChannel> upJob = std::make_unique<JobReqJoinChannel>(netId, req.m_channelId);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqLobbyChat(uint64_t netId, winppy::Packet packet)
{
	CSReqLobbyChat req;
	if (!packet->ReadBytes(&req, sizeof(req)))
	{
		Disconnect(netId);
		return;
	}

	if (req.m_msgLen > MAX_CHAT_MSG_LEN)
	{
		Disconnect(netId);
		return;
	}

	wchar_t chatMsg[MAX_CHAT_MSG_LEN + 1];
	if (!packet->ReadBytes(chatMsg, sizeof(wchar_t) * req.m_msgLen))
	{
		Disconnect(netId);
		return;
	}
	chatMsg[req.m_msgLen] = L'\0';

	std::unique_ptr<JobReqLobbyChat> upJob = std::make_unique<JobReqLobbyChat>(netId, req.m_msgLen, chatMsg);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqGameRoomList(uint64_t netId, winppy::Packet packet)
{
	CSReqGameRoomList req;
	if (!packet->ReadBytes(&req, sizeof(req)))
	{
		Disconnect(netId);
		return;
	}

	std::unique_ptr<JobReqGameRoomList> upJob = std::make_unique<JobReqGameRoomList>(netId, req.m_reqContextNo);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqCreateGameRoom(uint64_t netId, winppy::Packet packet)
{
	CSReqCreateGameRoom req;
	if (!packet->ReadBytes(&req, sizeof(req)))
	{
		Disconnect(netId);
		return;
	}

	const uint8_t tf = static_cast<uint8_t>(req.m_gameRoomTeamFormat);
	if (!(static_cast<uint8_t>(GameRoomTeamFormat::Team1vs1) <= tf && tf <= static_cast<uint8_t>(GameRoomTeamFormat::Team8vs8)))
	{
		Disconnect(netId);
		return;
	}

	if (req.m_gameRoomNameLen > MAX_GAME_ROOM_NAME_LEN)
	{
		Disconnect(netId);
		return;
	}

	wchar_t gameRoomName[MAX_GAME_ROOM_NAME_LEN + 1];
	if (!packet->ReadBytes(gameRoomName, sizeof(wchar_t) * req.m_gameRoomNameLen))
	{
		Disconnect(netId);
		return;
	}
	gameRoomName[req.m_gameRoomNameLen] = L'\0';

	std::unique_ptr<JobReqCreateGameRoom> upJob = std::make_unique<JobReqCreateGameRoom>(netId, req.m_gameRoomTeamFormat, req.m_gameRoomNameLen, gameRoomName);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqJoinGameRoom(uint64_t netId, winppy::Packet packet)
{
	CSReqJoinGameRoom req;
	if (!packet->ReadBytes(&req, sizeof(req)))
	{
		Disconnect(netId);
		return;
	}

	std::unique_ptr<JobReqJoinGameRoom> upJob = std::make_unique<JobReqJoinGameRoom>(netId, req.m_gameRoomId);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqChangeTeam(uint64_t netId, winppy::Packet packet)
{
	CSReqChangeTeam req;
	if (!packet->ReadBytes(&req, sizeof(req)))
	{
		Disconnect(netId);
		return;
	}

	std::unique_ptr<JobReqChangeTeam> upJob = std::make_unique<JobReqChangeTeam>(netId, req.m_newTeam);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqExitGameRoom(uint64_t netId, winppy::Packet packet)
{
	std::unique_ptr<JobReqExitGameRoom> upJob = std::make_unique<JobReqExitGameRoom>(netId);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqExitGameChannel(uint64_t netId, winppy::Packet packet)
{
	std::unique_ptr<JobReqExitChannel> upJob = std::make_unique<JobReqExitChannel>(netId);
	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqHostGameStart(uint64_t netId, winppy::Packet packet)
{
	// 
}

void GameServer::OnCSReqGameReady(uint64_t netId, winppy::Packet packet)
{
	std::unique_ptr<JobReqChangeGameReadyState> upJob =
		std::make_unique<JobReqChangeGameReadyState>(netId, true);

	m_logic.DispatchJob(std::move(upJob));
}

void GameServer::OnCSReqGameUnready(uint64_t netId, winppy::Packet packet)
{
	std::unique_ptr<JobReqChangeGameReadyState> upJob =
		std::make_unique<JobReqChangeGameReadyState>(netId, false);

	m_logic.DispatchJob(std::move(upJob));
}
