#pragma once

#include <cstdint>
#include "Constants.h"
#include "Contents.h"

using protocol_type = uint32_t;

enum class Protocol : protocol_type
{
	CS_REQ_LOGIN,
	CS_REQ_BROADCAST_CHAT_MSG,
	CS_REQ_GAME_LIST,

	SC_RES_LOGIN,
	SC_RES_BROADCAST_CHAT_MSG,
	SC_RES_GAME_LIST
};

struct CSReqLogin
{
	wchar_t m_id[MAX_ID_LEN];	// (NOT a null termination string)
	uint16_t m_idLen;
	unsigned char m_hpw[32];
};

struct CSReqBroadcastChatMsg
{
	uint16_t m_chatMsgLen;
	// chatMsg... (가변길이 데이터)
};

struct CSReqGameList
{
	uint32_t m_reqContextNo;	// 여러 패킷으로 전송될 수 있으므로 컨텍스트 번호를 전송.
};

struct SCResLogin
{
	bool m_result;	// 결과
	wchar_t m_nickname[MAX_NICKNAME_LEN];	// (NOT a null termination string)
	uint16_t m_level;	// 레벨
	uint32_t m_exp;		// 경험치
	uint32_t m_point;	// 포인트 소유량
};

struct SCResBroadcastChatMsg
{
	wchar_t m_nickname[MAX_NICKNAME_LEN];	// (NOT a null termination string)
	uint16_t m_chatMsgLen;
	// chatMsg... (가변길이 데이터)
};

struct SCResGameList
{
	uint32_t m_reqContextNo;		// 여러 패킷으로 전송될 수 있으므로 컨텍스트 번호를 전송.
	uint32_t m_itemCount;
};

struct SCResGameListItem
{
	uint64_t m_roomId;
	uint16_t m_gameNo;
	GameRoomMaxPlayer m_maxPlayer;
	uint8_t m_currPlayer;
	GameMap m_gameMap;
	GameMode m_gameMode;
	GameRoomState m_gameRoomState;
	uint16_t m_gameNameLen;
	wchar_t m_gameName[MAX_GAME_NAME_LEN];	// (NOT a null termination string)
};

// 압축 프로토콜들
#pragma pack(push, 1)

#pragma pack(pop)
