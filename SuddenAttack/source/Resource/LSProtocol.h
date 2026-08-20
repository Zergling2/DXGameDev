#pragma once

// Listen Server Protocol

#include "Constants.h"
#include "Contents.h"
#include <cstdint>

enum class LSProtocol : uint32_t
{
	CS_REQ_AUTH,
	CS_REQ_CHAT,
	CS_NOTIFY_GAME_PLAYER_TRANSFORM,

	SC_RES_AUTH_RESULT,
	SC_NOTIFY_GAME_STATUS,
	SC_NOTIFY_CHAT,
	SC_NOTIFY_GAME_PLAYER_JOINED,		// 새로운 플레이어의 입장을 알리는 용도.
	SC_NOTIFY_GAME_PLAYER_EXIT,
	SC_NOTIFY_GAME_PLAYER_INFO,			// 먼저 들어와있던 클라이언트의 정보들을 알려주는 용도.
	SC_NOTIFY_GAME_PLAYER_KILL,			// 킬 이벤트는 사망 정보를 암시적으로 내포
	SC_NOTIFY_GAME_PLAYER_DEAD,
	SC_NOTIFY_GAME_PLAYER_WEAPON_EVENT,	// DRAW, FIRE, RELOAD, ...
	SC_NOTIFY_GAME_PLAYER_TRANSFORM,	// 캐릭터 월드 트랜스폼 & 카메라 트랜스폼(위를 바라보는지 아래를 바라보는지 참고 & Additive Blending 수행)
	// SC_NOTIFY_DEAD,		// SC_NOTIFY_PLAYER_DEATH로 정보 대체 (자기 자신일 시 예외 처리 등)
	// SC_NOTIFY_PLAYER_DROP_WEAPON,
	SC_NOTIFY_START_RESPAWN,		// 서버에서 플레이어의 리스폰이 시작되었다는 정보 전달 목적 (수신 시 리스폰 UI 표시)
	SC_NOTIFY_GAME_PLAYER_RESPAWN,
	SC_NOTIFY_GAME_PLAYER_MOVEMENT_STATE_CHANGED,
	SC_NOTIFY_GAME_END
};


// LISTEN SERVER PACKET BASE STRUCT
struct LSPacketBase
{
	LSProtocol m_protocol;
};

struct LSCSReqAuth : public LSPacketBase
{
	uint32_t m_key;
	uint32_t m_accountId;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN];	// not a null termination string
	uint16_t m_level;
};

struct LSCSReqChat : public LSPacketBase
{
	uint32_t m_accountId;
	uint16_t m_chatMsgLen;
	wchar_t m_chatMsg[MAX_CHAT_MSG_LEN];	// not a null termination string
};

struct LSCSNotifyGamePlayerTransform : public LSPacketBase
{
	float m_x;
	float m_y;
	float m_z;
	float m_rx;
	float m_ry;
	float m_rz;
	float m_rw;
	float m_camRotX;
};

struct LSSCResAuthResult : public LSPacketBase
{
	bool m_result;
};

struct LSSCNotifyGameStatus : public LSPacketBase
{
	float m_gameRemainingTime;
};

struct LSSCNotifyChat : public LSPacketBase
{
	uint32_t m_accountId;
	uint16_t m_chatMsgLen;
	wchar_t m_chatMsg[MAX_CHAT_MSG_LEN];
};

struct LSSCNotifyGamePlayerJoined : public LSPacketBase
{
	uint32_t m_accountId;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN];
	GameTeam m_team;
	uint16_t m_level;
	uint32_t m_kill;
	uint32_t m_death;
	uint32_t m_ping;
	InGamePlayerState m_state;
	float m_x;
	float m_y;
	float m_z;
	float m_rx;
	float m_ry;
	float m_rz;
	float m_rw;
	float m_camRotX;
};

struct LSSCNotifyGamePlayerExit : public LSPacketBase
{
	uint32_t m_accountId;
};

struct LSSCNotifyGamePlayerInfo : public LSPacketBase
{
	uint32_t m_accountId;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN];
	GameTeam m_team;
	uint16_t m_level;
	uint32_t m_kill;
	uint32_t m_death;
	uint32_t m_ping;
	InGamePlayerState m_state;
	float m_x;
	float m_y;
	float m_z;
	float m_rx;
	float m_ry;
	float m_rz;
	float m_rw;
	float m_camRotX;
};

struct LSSCNotifyGamePlayerKill : public LSPacketBase
{
	uint32_t m_killerAccountId;
	uint32_t m_deaderAccountId;
	WeaponCode m_weapon;
};

struct LSSCNotifyGamePlayerDead : public LSPacketBase
{
	uint32_t m_deaderAccountId;
	uint32_t m_reason;
};

enum class LSNotifyWeaponEventType : uint8_t
{
	Draw,
	Fire,
	Reload
};

struct LSSCNotifyWeaponEvent : public LSPacketBase
{
	LSNotifyWeaponEventType m_type;
	WeaponCode m_weapon;
};

struct LSSCNotifyGamePlayerTransform : public LSPacketBase
{
	uint32_t m_accountId;
	float m_x;
	float m_y;
	float m_z;
	float m_rx;
	float m_ry;
	float m_rz;
	float m_rw;
	float m_camRotX;
};

struct LSSCNotifyStartRespawn : public LSPacketBase
{
	float m_remainingTime;
};

struct LSSCNotifyGamePlayerRespawn : public LSPacketBase
{
	uint32_t m_accountId;
	float m_x;
	float m_y;
	float m_z;
	float m_rx;
	float m_ry;
	float m_rz;
	float m_rw;
	float m_camRotX;
};

struct LSSCNotifyGamePlayerMovementStateChanged : public LSPacketBase
{
	bool m_w;
	bool m_a;
	bool m_s;
	bool m_d;
	bool m_moving;
	bool m_walk;
};
