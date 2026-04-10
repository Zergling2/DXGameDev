#include "ChJobReqCreateGameRoom.h"
#include "GameServer.h"
#include "GameSession.h"
#include "GameRoom.h"
#include "Protocol.h"
#include <cassert>

void ChJobReqCreateGameRoom::Execute(GameChannel& channel)
{
	bool disconnect = true;

	do
	{
		CSReqCreateGameRoom req;
		if (!m_packet->ReadBytes(&req, sizeof(req)))
			break;

		if (static_cast<uint8_t>(req.m_maxPlayer) > static_cast<uint8_t>(GameRoomMaxPlayer::Game8vs8))	// 유효하지 않은 enum 값 검사
			break;

		if (req.m_gameRoomNameLen > MAX_GAME_ROOM_NAME_LEN)	// 방 제목 요청 길이 검사
			break;

		wchar_t gameRoomName[MAX_GAME_ROOM_NAME_LEN + 1];
		if (!m_packet->ReadBytes(gameRoomName, sizeof(wchar_t) * req.m_gameRoomNameLen))
			break;

		gameRoomName[req.m_gameRoomNameLen] = L'\0';	// C 문자열화

		constexpr GameMap DEFAULT_GAME_MAP = GameMap::Warehouse;
		constexpr GameMode DEFAULT_GAME_MODE = GameMode::TeamDeathmatch;
		
		const uint64_t gameRoomId = channel.CreateGameRoomId();
		const uint16_t gameRoomNo = channel.CreateGameRoomNo();
		std::shared_ptr<GameRoom> spGameRoom = std::make_shared<GameRoom>(gameRoomId, gameRoomNo, req.m_maxPlayer,
			DEFAULT_GAME_MAP, DEFAULT_GAME_MODE, gameRoomName, req.m_gameRoomNameLen);
		channel.GameRooms().insert(std::make_pair(gameRoomId, spGameRoom));

		spGameRoom->m_spHost = m_spSession;	// 방장 참조 설정

		bool result = spGameRoom->AddSession(m_spSession);	// 방 자료구조에 세션 추가
		m_spSession->SetJoiningGameRoom(spGameRoom);	// 방 생성을 요청한 세션의 입장 방 포인터 업데이트
		assert(result);

		// 결과 패킷 전송
		SCResCreateGameRoom res;
		res.m_result = true;
		res.m_gameRoomId = gameRoomId;
		res.m_gameRoomNo = gameRoomNo;
		res.m_gameRoomHostNetId = m_spSession->GetNetId();
		res.m_maxPlayer = spGameRoom->GetMaxPlayer();
		res.m_gameMap = spGameRoom->GetGameMap();
		res.m_gameMode = spGameRoom->GetGameMode();
		res.m_gameRoomNameLen = spGameRoom->GetNameLen();
		res.m_joinedTeam = m_spSession->GetCurrTeam();

		winppy::Packet outPacket;
		outPacket->Write(static_cast<protocol_type>(Protocol::SC_RES_CREATE_GAME_ROOM));
		outPacket->WriteBytes(&res, sizeof(res));
		outPacket->WriteBytes(gameRoomName, sizeof(wchar_t) * req.m_gameRoomNameLen);

		m_server.Send(m_spSession->GetNetId(), std::move(outPacket));

		disconnect = false;
	} while (false);

	if (disconnect)
		m_server.Disconnect(m_spSession->GetNetId());
}
