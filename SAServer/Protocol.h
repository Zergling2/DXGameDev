#pragma once

#include <cstdint>
#include "Constants.h"
#include "Contents.h"

using protocol_type = uint32_t;

enum class Protocol : protocol_type
{
	CS_REQ_LOGIN,
	CS_REQ_CHANNEL_INFO,
	CS_REQ_JOIN_CHANNEL,
	CS_REQ_SEND_CHAT_MSG,
	CS_REQ_GAME_LIST,
	CS_REQ_CREATE_GAME_ROOM,
	CS_REQ_JOIN_GAME_ROOM,
	CS_REQ_CHANGE_TEAM,
	CS_REQ_EXIT_GAME_ROOM,
	CS_REQ_HOST_GAME_START,

	SC_RES_LOGIN,
	SC_RES_CHANNEL_INFO,
	SC_RES_JOIN_CHANNEL,
	SC_RES_SEND_CHAT_MSG,
	SC_RES_GAME_LIST,
	SC_RES_CREATE_GAME_ROOM,
	SC_RES_JOIN_GAME_ROOM,
	SC_RES_CHANGE_TEAM,
	SC_RES_EXIT_GAME_ROOM,
	SC_NOTIFY_PLAYER_TEAM_CHANGED,
	SC_NOTIFY_PLAYER_JOINED,
	SC_NOTIFY_GAME_ROOM_PLAYER,
	SC_NOTIFY_HOST_GAME_START
};

struct CSReqLogin
{
	uint16_t m_idLen;
	wchar_t m_id[MAX_ID_LEN];	// (NOT a null termination string)
	unsigned char m_hpw[32];
};

struct CSReqChannelInfo
{
	uint16_t m_serverId;	// 서버군 식별자
};

struct CSReqJoinChannel
{
	uint16_t m_channelId;
};

struct CSReqSendChatMsg
{
	uint16_t m_chatMsgLen;
	// chatMsg... (가변길이 데이터)
};

struct CSReqGameRoomList
{
	uint32_t m_reqContextNo;	// 여러 패킷으로 전송될 수 있으므로 컨텍스트 번호를 전송.
};

struct CSReqCreateGameRoom
{
	GameRoomMaxPlayer m_maxPlayer;
	uint16_t m_gameRoomNameLen;
	// m_gameRoomName... (가변길이 데이터)
};

struct CSReqJoinGameRoom
{
	uint64_t m_gameRoomId;
};

struct CSReqChangeTeam
{
	GameTeam m_newTeam;
};

struct SCResLogin
{
	bool m_result;	// 결과
	uint64_t m_netId;
	uint64_t m_accountId;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN];	// (NOT a null termination string)
	uint16_t m_level;	// 레벨
	uint32_t m_exp;		// 경험치
	uint32_t m_point;	// 포인트 소유량
};

struct SCResChannelInfo
{
	uint16_t m_serverId;	// 서버군 식별자
	uint16_t m_channelId;	// 채널 식별자
	uint16_t m_sessionCount;
	uint16_t m_maxSession;
};

struct SCResJoinChannel
{
	bool m_result;
};

struct SCResSendChatMsg
{
	uint16_t m_nicknameLen;
	uint16_t m_chatMsgLen;
	// nickname... (가변길이 데이터)
	// chatMsg... (가변길이 데이터)
};

struct SCResGameList
{
	uint32_t m_reqContextNo;		// 여러 패킷으로 전송될 수 있으므로 컨텍스트 번호를 전송.
};

struct SCResGameListItem
{
	uint64_t m_gameRoomId;
	uint16_t m_gameRoomNo;
	GameRoomMaxPlayer m_maxPlayer;
	uint8_t m_currPlayer;
	GameMap m_gameMap;
	GameMode m_gameMode;
	GameRoomState m_gameRoomState;
	uint16_t m_gameRoomNameLen;
	wchar_t m_gameName[MAX_GAME_ROOM_NAME_LEN];	// (NOT a null termination string)
};

struct SCResCreateGameRoom
{
	bool m_result;
	uint64_t m_gameRoomId;
	uint16_t m_gameRoomNo;
	uint64_t m_gameRoomHostNetId;
	GameRoomMaxPlayer m_maxPlayer;
	GameMap m_gameMap;
	GameMode m_gameMode;
	GameTeam m_joinedTeam;
	uint16_t m_gameRoomNameLen;
	// m_gameName... (가변길이 데이터)
};

enum class FailReason
{
	InvalidGame,
	Full,

	Success
};

struct SCResJoinGameRoom
{
	bool m_result;
	FailReason m_reason;
	uint64_t m_gameRoomId;
	uint16_t m_gameRoomNo;
	uint64_t m_gameRoomHostNetId;
	GameRoomMaxPlayer m_maxPlayer;
	GameMap m_gameMap;
	GameMode m_gameMode;
	GameTeam m_joinedTeam;
	uint16_t m_gameRoomNameLen;
	wchar_t m_gameRoomName[MAX_GAME_ROOM_NAME_LEN];
};

struct SCNotifyGameRoomPlayer
{
	uint64_t m_gameRoomId;
	uint64_t m_netId;
	GameTeam m_team;
	uint16_t m_level;	// 레벨
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN];
};

struct SCResChangeTeam
{
	bool m_result;
	GameTeam m_newTeam;
};

struct SCNotifyPlayerJoined
{
	uint64_t m_gameRoomId;
	uint64_t m_netId;
	GameTeam m_joinedTeam;
	uint16_t m_level;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN];
};

struct SCNotifyPlayerTeamChanged
{
	uint64_t m_netId;
	uint64_t m_gameRoomId;
	GameTeam m_newTeam;
};

// 압축 프로토콜들
#pragma pack(push, 1)

#pragma pack(pop)
