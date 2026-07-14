#pragma once

#include <cstdint>
#include "Constants.h"
#include "Contents.h"

using protocol_type = uint32_t;

enum class Protocol : protocol_type
{
	CS_REQ_LOGIN,
	CS_REQ_ID_DUPLICATE_CHECK,
	CS_REQ_NICKNAME_DUPLICATE_CHECK,
	CS_REQ_CREATE_ACCOUNT,
	CS_REQ_CHANNEL_INFO,
	CS_REQ_JOIN_CHANNEL,
	CS_REQ_LOBBY_CHAT,
	CS_REQ_GAME_ROOM_LIST,
	CS_REQ_CREATE_GAME_ROOM,
	CS_REQ_JOIN_GAME_ROOM,
	CS_REQ_CHANGE_TEAM,
	CS_REQ_EXIT_GAME_ROOM,
	CS_REQ_HOST_GAME_STARTABLE_STATE,
	CS_REQ_GAME_READY,
	CS_REQ_GAME_UNREADY,
	CS_REQ_EXIT_GAME_CHANNEL,

	SC_RES_LOGIN,
	SC_RES_ID_DUPLICATE_CHECK,
	SC_RES_NICKNAME_DUPLICATE_CHECK,
	SC_RES_CREATE_ACCOUNT,
	SC_RES_CHANNEL_INFO,
	SC_RES_JOIN_CHANNEL,
	SC_RES_GAME_ROOM_LIST,
	SC_RES_CREATE_GAME_ROOM,
	SC_RES_JOIN_GAME_ROOM,
	SC_RES_HOST_GAME_STARTABLE_STATE,
	SC_RES_EXIT_GAME_ROOM,
	SC_RES_EXIT_GAME_CHANNEL,
	SC_NOTIFY_LOBBY_CHAT,
	SC_NOTIFY_PLAYER_TEAM_CHANGED,
	SC_NOTIFY_PLAYER_JOINED_GAME_ROOM,
	SC_NOTIFY_GAME_ROOM_PLAYER,
	SC_NOTIFY_PLAYER_EXIT_GAME_ROOM,
	SC_NOTIFY_HOST_CHANGED,
	SC_NOTIFY_HOST_GAME_STARTED,
	SC_NOTIFY_PLAYER_STATE_CHANGED
};

enum class JoinGameRoomResult : uint8_t
{
	Success,
	InvalidGame,
	Full,
};

struct ChannelInfo
{
	uint16_t m_numOfPlayers;
	uint16_t m_maxPlayerCount;
};

struct CSReqLogin
{
	uint16_t m_idLen;
	wchar_t m_id[MAX_ID_LEN];	// (NOT a null termination string)
	uint16_t m_pwLen;
	wchar_t m_pw[MAX_PW_LEN];	// (NOT a null termination string)
};

struct CSReqIdDuplicateCheck
{
	uint16_t m_idLen;
	wchar_t m_id[MAX_ID_LEN];	// (NOT a null termination string)
};

struct CSReqNicknameDuplicateCheck
{
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN];	// (NOT a null termination string)
};

struct CSReqCreateAccount
{
	uint16_t m_idLen;
	wchar_t m_id[MAX_ID_LEN];	// (NOT a null termination string)
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_ID_LEN];	// (NOT a null termination string)
	uint16_t m_pwLen;
	wchar_t m_pw[MAX_PW_LEN];	// (NOT a null termination string)
};

struct CSReqChannelInfo
{
	uint16_t m_worldId;	// 서버군 식별자
};

struct CSReqJoinChannel
{
	uint8_t m_channelId;	// game channel id
};

struct CSReqLobbyChat
{
	uint16_t m_msgLen;
	// msg... (가변길이 데이터)
};

struct CSReqGameRoomList
{
	uint32_t m_reqContextNo;	// 여러 패킷으로 전송될 수 있으므로 컨텍스트 번호를 전송.
};

struct CSReqCreateGameRoom
{
	GameRoomTeamFormat m_gameRoomTeamFormat;
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
	bool m_querySuccess;
	bool m_result;	// 결과
	uint32_t m_accountId;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN];	// (NOT a null termination string)
	uint16_t m_level;	// 레벨
	uint32_t m_exp;		// 경험치
	uint32_t m_point;	// 포인트 소유량
};

struct SCResIdDuplicateCheck
{
	bool m_querySuccess;	// 서버에서 조회에 성공하였는지 나타냄. false이면 m_duplicated가 유효하지 않음.
	bool m_duplicated;
};

struct SCResNicknameDuplicateCheck
{
	bool m_querySuccess;	// 서버에서 조회에 성공하였는지 나타냄. false이면 m_duplicated가 유효하지 않음.
	bool m_duplicated;
};

struct SCResCreateAccount
{
	bool m_result;
};

struct SCResChannelInfo
{
	uint16_t m_worldId;	// 서버군 식별자
	ChannelInfo m_channelInfo[CHANNEL_COUNT];
};

struct SCResJoinChannel
{
	bool m_result;
};

struct SCNotifyLobbyChat
{
	uint32_t m_accountId;
	uint16_t m_nicknameLen;
	uint16_t m_msgLen;
	// nickname... (가변길이 데이터)
	// msg... (가변길이 데이터)
};

struct SCResGameRoomList
{
	uint32_t m_reqContextNo;		// 여러 패킷으로 전송될 수 있으므로 컨텍스트 번호를 전송.
};

struct SCResGameRoomListItem
{
	uint64_t m_id;
	uint16_t m_no;
	GameRoomTeamFormat m_tf;
	uint8_t m_numOfPlayers;
	GameMap m_map;
	GameRoomState m_state;
	uint16_t m_nameLen;
	wchar_t m_name[MAX_GAME_ROOM_NAME_LEN];	// (NOT a null termination string)
};

struct SCResCreateGameRoom
{
	bool m_result;
	uint64_t m_gameRoomId;
	uint16_t m_gameRoomNo;
	uint32_t m_gameRoomHostAccountId;
	GameRoomTeamFormat m_gameRoomTeamFormat;
	GameMap m_gameMap;
	GameTeam m_joinedTeam;
	uint16_t m_gameRoomNameLen;
	wchar_t m_gameRoomName[MAX_GAME_ROOM_NAME_LEN];
};

struct SCResJoinGameRoom
{
	JoinGameRoomResult m_result;
	uint64_t m_gameRoomId;
	uint16_t m_gameRoomNo;
	uint32_t m_gameRoomHostAccountId;
	GameRoomTeamFormat m_gameRoomTeamFormat;
	GameMap m_gameMap;
	GameTeam m_joinedTeam;
	uint16_t m_gameRoomNameLen;
	wchar_t m_gameRoomName[MAX_GAME_ROOM_NAME_LEN];
};

enum class HostGameStartableState : uint8_t
{
	Startable,
	NotReady	// ex) 상대팀이 한 명 이상 준비하지 않은 경우
};

struct SCResHostGameStartableState
{
	HostGameStartableState m_result;
	GameTeam m_joinedTeam;
	GameMap	m_map;
};

struct SCResExitGameRoom
{
	bool m_result;
};

struct SCResExitGameChannel
{
	bool m_result;
};

struct SCNotifyPlayerTeamChanged
{
	uint32_t m_accountId;
	GameTeam m_newTeam;
};

struct SCNotifyPlayerJoined
{
	uint32_t m_accountId;
	GameTeam m_team;
	uint16_t m_level;
	PlayerState m_state;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN];
};

struct SCNotifyHostChanged
{
	uint32_t m_oldHostAccountId;
	PlayerState m_oldHostNewState;
	uint32_t m_newHostAccountId;
	PlayerState m_newHostNewState;
};

struct SCNotifyPlayerStateChanged
{
	uint32_t m_accountId;
	PlayerState m_newState;
};

// 방에 먼저 입장해있던 플레이어들의 정보를 알려주는 패킷
struct SCNotifyGameRoomPlayer
{
	uint32_t m_accountId;
	GameTeam m_team;
	uint16_t m_level;	// 레벨
	PlayerState m_state;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN];
};

struct SCNotifyPlayerExitGameRoom
{
	uint32_t m_accountId;
};

struct SCNotifyHostGameStarted
{
	uint32_t m_hostGameAddr;
	uint16_t m_hostGamePort;
};

// 압축 프로토콜들
#pragma pack(push, 1)
// ...
// ...
// ...
#pragma pack(pop)
