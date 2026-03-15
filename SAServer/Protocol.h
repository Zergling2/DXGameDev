#pragma once

#include <cstdint>
#include "Constants.h"

using protocol_type = uint32_t;

enum class Protocol : protocol_type
{
	CS_REQ_LOGIN,
	CS_REQ_BROADCAST_CHAT_MSG,


	SC_RES_LOGIN,
	SC_RES_BROADCAST_CHAT_MSG,
};

struct CSReqLogin
{
	wchar_t m_id[MAX_ID_LEN];	// (NOT a null terminating string)
	uint16_t m_idLen;
	unsigned char m_hpw[32];
};

struct CSReqBroadcastChatMsg
{
	uint16_t m_chatMsgLen;
	// chatMsg... (가변길이 데이터)
};

struct SCResLogin
{
	bool m_result;	// 결과
	wchar_t m_nickname[MAX_NICKNAME_LEN];	// (NOT a null terminating string)
	uint16_t m_level;	// 레벨
	uint32_t m_exp;		// 경험치
	uint32_t m_point;	// 포인트 소유량
};

struct SCResBroadcastChatMsg
{
	wchar_t m_nickname[MAX_NICKNAME_LEN];	// (NOT a null terminating string)
	uint16_t m_chatMsgLen;
	// chatMsg... (가변길이 데이터)
};

// 압축 프로토콜들
#pragma pack(push, 1)

#pragma pack(pop)
