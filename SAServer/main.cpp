#include <winppy/Network/TCPServer.h>
#include <winppy/Network/Packet.h>
#include "SlimRWLock.h"
#include "SASession.h"
#include "Protocol.h"
#include <unordered_map>
#include <memory>

class SAServer : public winppy::TCPServer
{
public:
	SAServer();
	virtual ~SAServer() = default;
	virtual bool OnConnect(const wchar_t* ip, uint16_t port, uint64_t id) override;
	virtual void OnReceive(uint64_t id, winppy::Packet packet) override;
	virtual void OnDisconnect(uint64_t id) override;

	bool PktProcCSReqLogin(uint64_t id, winppy::Packet packet);
	bool PktProcCSReqSendChatMsg(uint64_t id, winppy::Packet packet);
	bool PktProcCSReqGameList(uint64_t id, winppy::Packet packet);
private:
	SlimRWLock m_lock;
	std::unordered_map<uint64_t, std::shared_ptr<SASession>> m_sessions;
};

bool SAServer::PktProcCSReqLogin(uint64_t id, winppy::Packet packet)
{
	CSReqLogin req;

	if (!packet->ReadBytes(&req, sizeof(req)))
		return false;

	if (req.m_idLen > MAX_ID_LEN)
		return false;

	wchar_t idStr[MAX_ID_LEN + 1];
	wmemcpy(idStr, req.m_id, req.m_idLen);
	idStr[req.m_idLen] = L'\0';

	wprintf(L"id: %ls, hpw: ", idStr);
	for (size_t i = 0; i < 32; ++i)
	{
		wprintf(L"%02x", req.m_hpw[i]);
	}
	wprintf(L"\n");



	// 수행해야 할 작업
	// DB 조회.
	// 일치 항목 조회 성공 시 세션의 인증 플래그 true로 켜기
	// DB로부터 읽어온 세션 정보들을 SASession 객체에 대입s
	// 세션에게 연결 성공 패킷 전송
	// 실패 시 실패 패킷 전송

	// Test 코드
	SCResLogin res;
	res.m_result = false;
	{
		AutoAcquireSlimRWLockExclusive lock(m_lock);

		auto iter = m_sessions.find(id);
		assert(iter != m_sessions.cend());
		if (iter == m_sessions.cend())
			return false;

		SASession& session = *iter->second.get();
		session.SetAuthenticated();
		session.SetLevel(1);
		session.SetExp(500);
		session.SetPoint(5000);
		session.SetNickname(L"감자튀김");
		
		res.m_result = true;
		wmemcpy_s(res.m_nickname, _countof(res.m_nickname), session.GetNickname(), MAX_NICKNAME_LEN);
		res.m_level = session.GetLevel();
		res.m_exp = session.GetExp();
		res.m_point = session.GetPoint();
	}

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_LOGIN));
	outPacket->WriteBytes(&res, sizeof(res));
	this->Send(id, std::move(outPacket));

	return true;
}

bool SAServer::PktProcCSReqSendChatMsg(uint64_t id, winppy::Packet packet)
{
	CSReqBroadcastChatMsg req;
	if (!packet->ReadBytes(&req, sizeof(req)))
		return false;

	if (req.m_chatMsgLen > MAX_CHAT_MSG_LEN)
		return false;

	wchar_t chatMsg[MAX_CHAT_MSG_LEN];
	if (!packet->ReadBytes(chatMsg, sizeof(wchar_t) * req.m_chatMsgLen))
		return false;

	AutoAcquireSlimRWLockExclusive lock(m_lock);

	auto iter = m_sessions.find(id);
	assert(iter != m_sessions.cend());
	if (iter == m_sessions.cend())
		return false;

	const SASession& sender = *iter->second.get();

	if (!sender.GetAuthenticated())
		return false;

	SCResBroadcastChatMsg res;
	wmemcpy_s(res.m_nickname, _countof(res.m_nickname), sender.GetNickname(), MAX_NICKNAME_LEN);
	res.m_chatMsgLen = req.m_chatMsgLen;

	winppy::Packet outPacket;
	outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_BROADCAST_CHAT_MSG));
	outPacket->WriteBytes(&res, sizeof(res));
	outPacket->WriteBytes(chatMsg, sizeof(wchar_t) * res.m_chatMsgLen);

	for (const auto& pair : m_sessions)
	{
		const SASession& session = *pair.second.get();

		this->Send(session.GetNetId(), outPacket);	// 채널의 모든 유저들에게 전송
	}

	// ...

	return true;
}

bool SAServer::PktProcCSReqGameList(uint64_t id, winppy::Packet packet)
{
	CSReqGameList req;
	if (!packet->ReadBytes(&req, sizeof(req)))
		return false;

	for (size_t r = 0; r < 4; ++r)
	{
		SCResGameList res;
		res.m_itemCount = 10;
		res.m_reqContextNo = req.m_reqContextNo;

		winppy::Packet outPacket;
		outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_GAME_LIST));
		outPacket->WriteBytes(&res, sizeof(res));

		for (uint32_t i = 0; i < res.m_itemCount; ++i)
		{
			SCResGameListItem resItem;

			resItem.m_roomId = 1234;
			resItem.m_gameNo = 3;
			resItem.m_maxPlayer = GameRoomMaxPlayer::Game8vs8;
			resItem.m_currPlayer = 2;
			resItem.m_gameMap = GameMap::Warehouse;
			resItem.m_gameMode = GameMode::TeamDeathmatch;
			resItem.m_gameRoomState = GameRoomState::InPlay;
			resItem.m_gameNameLen = 9;
			wmemcpy(resItem.m_gameName, L"Go go go!", 9);
			outPacket->WriteBytes(&resItem, sizeof(resItem));
		}

		Send(id, std::move(outPacket));
	}

	return true;
}

SAServer::SAServer()
{
	m_lock.Init();
}

bool SAServer::OnConnect(const wchar_t* ip, uint16_t port, uint64_t id)
{
	wprintf(L"Remote address: %ls:%u connected.\n", ip, static_cast<uint32_t>(port));

	m_lock.AcquireLockExclusive();
	m_sessions.insert(std::make_pair(id, std::make_shared<SASession>(id)));
	m_lock.ReleaseLockExclusive();

	return true;
}

void SAServer::OnReceive(uint64_t id, winppy::Packet packet)
{
	Protocol protocol;

	if (packet->ReadableSize() < sizeof(protocol))
	{
		Disconnect(id);
		return;
	}

	packet->Read(reinterpret_cast<protocol_type*>(&protocol));

	bool keepConn;
	switch (protocol)
	{
	case Protocol::CS_REQ_LOGIN:
		keepConn = PktProcCSReqLogin(id, std::move(packet));
		break;
	case Protocol::CS_REQ_BROADCAST_CHAT_MSG:
		keepConn = PktProcCSReqSendChatMsg(id, std::move(packet));
		break;
	case Protocol::CS_REQ_GAME_LIST:
		keepConn = PktProcCSReqGameList(id, std::move(packet));
		break;
	default:
		keepConn = false;
		break;
	}

	if (!keepConn)
		Disconnect(id);
}

void SAServer::OnDisconnect(uint64_t id)
{
	wprintf(L"Session %llu Disconnected.\n", id);

	m_lock.AcquireLockExclusive();
	m_sessions.erase(id);
	m_lock.ReleaseLockExclusive();
}

int main(void)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return -1;

	SAServer server;

	winppy::TCPServerConfig config;
	config.m_bindAddr = nullptr;
	config.m_bindPort = SASERVER_PORT;
	config.m_numOfWorkerThreads = 6;
	config.m_numOfConcurrentThreads = 4;
	config.m_tcpNoDelay = true;
	config.m_headerCode = HEADER_CODE;
	config.m_maxSessionCount = 5000;
	int ret = server.Run(config);
	if (ret == 0)
		server.Shutdown();

	WSACleanup();

	return 0;
}
