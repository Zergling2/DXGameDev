#include "GameServer.h"
#include "Protocol.h"
#include "GameSession.h"
#include "ChJobReqChannelInfo.h"
#include "ChJobReqJoinChannel.h"
#include "ChJobReqSendChatMsg.h"
#include "ChJobReqGameRoomList.h"
#include "ChJobReqCreateGameRoom.h"
#include "ChJobReqJoinGameRoom.h"
#include "ChJobReqExitGameRoom.h"
#include "ChJobReqExitGameChannel.h"
#include "ChJobReqChangeTeam.h"

GameServer::GameServer()
{
	for (size_t i = 0; i < _countof(m_channel); ++i)
		m_channel[i].Init(static_cast<uint16_t>(i));
}

bool GameServer::OnConnect(const wchar_t* ip, uint16_t port, uint64_t id)
{
	wprintf(L"Remote address: %s:%u connected.\n", ip, static_cast<uint32_t>(port));

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

	bool keepConn = false;
	if (protocol == Protocol::CS_REQ_LOGIN)
	{
		keepConn = PktProcCSReqLogin(id, std::move(packet));
	}
	else
	{
		std::shared_ptr<GameSession> spSession = FindSession(id);

		if (spSession != nullptr)
		{
			switch (protocol)
			{
			case Protocol::CS_REQ_CHANNEL_INFO:
				keepConn = PktProcCSReqChannelInfo(id, std::move(packet));
				break;
			case Protocol::CS_REQ_JOIN_CHANNEL:
				keepConn = PktProcCSReqJoinChannel(std::move(packet), std::move(spSession));
				break;
			case Protocol::CS_REQ_SEND_CHAT_MSG:
				keepConn = PktProcCSReqSendChatMsg(std::move(packet), std::move(spSession));
				break;
			case Protocol::CS_REQ_GAME_LIST:
				keepConn = PktProcCSReqGameList(id, std::move(packet), std::move(spSession));
				break;
			case Protocol::CS_REQ_CREATE_GAME_ROOM:
				keepConn = PktProcCSReqCreateGameRoom(std::move(packet), std::move(spSession));
				break;
			case Protocol::CS_REQ_JOIN_GAME_ROOM:
				keepConn = PktProcCSReqJoinGameRoom(std::move(packet), std::move(spSession));
				break;
			case Protocol::CS_REQ_CHANGE_TEAM:
				keepConn = PktProcCSReqChangeTeam(std::move(packet), std::move(spSession));
				break;
			case Protocol::CS_REQ_EXIT_GAME_ROOM:
				keepConn = PktProcCSReqExitGameRoom(std::move(spSession));
				break;
			case Protocol::CS_REQ_HOST_GAME_START:
				break;
			default:
				keepConn = false;
				break;
			}
		}
		else
		{
			keepConn = false;
		}
	}

	if (!keepConn)
		Disconnect(id);
}

void GameServer::OnDisconnect(uint64_t id)
{
	wprintf(L"Session %llu Disconnected.\n", id);

	std::shared_ptr<GameSession> spSession = FindSession(id);
	if (!spSession)
		return;

	spSession->SetDisconnectedFlag();

	GameChannel* pChannel = spSession->GetJoiningGameChannel();
	if (pChannel)
	{
		// pChannel->DispatchJob();	// 채널에서 세션 아웃 작업 진행.
	}
}

bool GameServer::PktProcCSReqLogin(uint64_t id, winppy::Packet packet)
{
	CSReqLogin req;

	if (!packet->ReadBytes(&req, sizeof(req)))
		return false;

	if (req.m_idLen > MAX_ID_LEN)
		return false;

	wchar_t idStr[MAX_ID_LEN + 1];
	wmemcpy(idStr, req.m_id, req.m_idLen);
	idStr[req.m_idLen] = L'\0';

	wprintf(L"id: %s, hpw: ", idStr);
	for (size_t i = 0; i < 32; ++i)
	{
		wprintf(L"%02x", req.m_hpw[i]);
	}
	wprintf(L"\n");

	constexpr uint64_t TEST_ACCOUNT_ID = 0xffffffff00000000;
	std::shared_ptr<GameSession> spSession = std::make_shared<GameSession>(id, TEST_ACCOUNT_ID);
	spSession->SetNickname(L"감자튀김", static_cast<uint16_t>(wcslen(L"감자튀김")));
	spSession->SetLevel(1);
	spSession->SetExp(500);
	spSession->SetPoint(5000);


	SCResLogin res;
	res.m_result = true;
	res.m_netId = id;
	res.m_accountId = TEST_ACCOUNT_ID;
	res.m_nicknameLen = spSession->GetNicknameLen();
	wmemcpy_s(res.m_nickname, _countof(res.m_nickname), spSession->GetNickname(), spSession->GetNicknameLen());
	res.m_level = spSession->GetLevel();
	res.m_exp = spSession->GetExp();
	res.m_point = spSession->GetPoint();


	{
		// 세션 정보 모두 세팅 후 세션 맵에 추가.
		AutoAcquireSlimRWLockExclusive lock(m_sessionMapLock);
		m_sessions.insert(std::make_pair(id, spSession));
	}

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_LOGIN));
	outPacket->WriteBytes(&res, sizeof(res));
	this->Send(id, std::move(outPacket));

	return true;
}

bool GameServer::PktProcCSReqChannelInfo(uint64_t id, winppy::Packet packet)
{
	for (size_t i = 0; i < _countof(m_channel) - 1; ++i)
		m_channel[i].DispatchJob(std::make_unique<ChJobReqChannelInfo>(*this, id, packet));

	m_channel[_countof(m_channel) - 1].DispatchJob(std::make_unique<ChJobReqChannelInfo>(*this, id, std::move(packet)));	// 마지막 채널에 대해서는 패킷 move

	return true;
}

bool GameServer::PktProcCSReqJoinChannel(winppy::Packet packet, std::shared_ptr<GameSession> spSession)
{
	CSReqJoinChannel req;
	if (!packet->ReadBytes(&req, sizeof(req)))
		return false;

	if (req.m_channelId >= CHANNEL_COUNT)
		return false;

	m_channel[req.m_channelId].DispatchJob(std::make_unique<ChJobReqJoinChannel>(*this, std::move(spSession)));

	return true;
}

bool GameServer::PktProcCSReqSendChatMsg(winppy::Packet packet, std::shared_ptr<GameSession> spSession)
{
	GameChannel* pJoiningGameChannel = spSession->GetJoiningGameChannel();
	if (!pJoiningGameChannel)
		return false;

	pJoiningGameChannel->DispatchJob(std::make_unique<ChJobReqSendChatMsg>(*this, std::move(packet), std::move(spSession)));
	return true;
}

bool GameServer::PktProcCSReqGameList(uint64_t id, winppy::Packet packet, std::shared_ptr<GameSession> spSession)
{
	GameChannel* pJoiningGameChannel = spSession->GetJoiningGameChannel();
	if (!pJoiningGameChannel)
		return false;

	pJoiningGameChannel->DispatchJob(std::make_unique<ChJobReqGameRoomList>(*this, id, std::move(packet)));
	return true;
}

bool GameServer::PktProcCSReqCreateGameRoom(winppy::Packet packet, std::shared_ptr<GameSession> spSession)
{
	GameChannel* pJoiningGameChannel = spSession->GetJoiningGameChannel();
	if (!pJoiningGameChannel)
		return false;

	pJoiningGameChannel->DispatchJob(std::make_unique<ChJobReqCreateGameRoom>(*this, std::move(packet), std::move(spSession)));
	return true;
}

bool GameServer::PktProcCSReqJoinGameRoom(winppy::Packet packet, std::shared_ptr<GameSession> spSession)
{
	GameChannel* pJoiningGameChannel = spSession->GetJoiningGameChannel();
	if (!pJoiningGameChannel)
		return false;

	pJoiningGameChannel->DispatchJob(std::make_unique<ChJobReqJoinGameRoom>(*this, std::move(packet), std::move(spSession)));
	return true;
}

bool GameServer::PktProcCSReqExitGameRoom(std::shared_ptr<GameSession> spSession)
{
	GameChannel* pJoiningGameChannel = spSession->GetJoiningGameChannel();
	if (!pJoiningGameChannel)
		return false;

	pJoiningGameChannel->DispatchJob(std::make_unique<ChJobReqExitGameRoom>(*this, std::move(spSession)));
	return true;
}

bool GameServer::PktProcCSReqExitGameChannel(std::shared_ptr<GameSession> spSession)
{
	GameChannel* pJoiningGameChannel = spSession->GetJoiningGameChannel();
	if (!pJoiningGameChannel)
		return false;

	pJoiningGameChannel->DispatchJob(std::make_unique<ChJobReqExitGameChannel>(*this, std::move(spSession)));
	return true;
}

bool GameServer::PktProcCSReqChangeTeam(winppy::Packet packet, std::shared_ptr<GameSession> spSession)
{
	GameChannel* pJoiningGameChannel = spSession->GetJoiningGameChannel();
	if (!pJoiningGameChannel)
		return false;

	pJoiningGameChannel->DispatchJob(std::make_unique<ChJobReqChangeTeam>(*this, std::move(packet), std::move(spSession)));
	return true;
}

std::shared_ptr<GameSession> GameServer::FindSession(uint64_t netId) const
{
	std::shared_ptr<GameSession> spSession;

	AutoAcquireSlimRWLockShared lock(m_sessionMapLock);

	const auto pair = m_sessions.find(netId);
	if (pair != m_sessions.cend())
		spSession = pair->second;

	return spSession;
}