#pragma once

#include <unordered_map>
#include "JobThreadBase.h"
#include "Constants.h"
#include "GameRoom.h"
#include "Channel.h"
#include "Session.h"
#include "Player.h"

class Session;
class LogicThread;

class IJob
{
public:
	IJob() = default;
	virtual ~IJob() = default;

	virtual void Execute(LogicThread& thread) = 0;
};

class JobCreateNewSession : public IJob
{
public:
	JobCreateNewSession(uint64_t netId)
		: m_netId(netId)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
};

class JobReqLogin : public IJob
{
public:
	JobReqLogin(uint64_t netId, const wchar_t* id, const wchar_t* pw);

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_id[MAX_ID_LEN + 1];
	wchar_t m_pw[MAX_PW_LEN + 1];
};

// DB스레드에서 날리는 잡 객체
// 인증 완료된 세션(로그인 성공)인 경우
class JobAuthSession : public IJob
{
public:
	JobAuthSession(uint64_t authSessionNetId)
		: m_authSessionNetId(authSessionNetId)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_authSessionNetId;
};

class JobReqIdDuplicateCheck : public IJob
{
public:
	JobReqIdDuplicateCheck(uint64_t netId, const wchar_t* id);

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_id[MAX_ID_LEN + 1];
};

class JobReqNicknameDuplicateCheck : public IJob
{
public:
	JobReqNicknameDuplicateCheck(uint64_t netId, const wchar_t* nickname);

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];
};

class JobReqChannelInfo : public IJob
{
public:
	JobReqChannelInfo(uint64_t netId)
		: m_netId(netId)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
};

class JobReqJoinChannel : public IJob
{
public:
	JobReqJoinChannel(uint64_t netId, uint8_t channelId)
		: m_netId(netId)
		, m_channelId(channelId)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	uint8_t m_channelId;
};

class JobReqExitChannel : public IJob
{
public:
	JobReqExitChannel(uint64_t netId)
		: m_netId(netId)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
};

class JobReqLobbyChat : public IJob
{
public:
	JobReqLobbyChat(uint64_t netId, uint16_t msgLen, const wchar_t* msg);

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	uint16_t m_msgLen;
	wchar_t m_msg[MAX_CHAT_MSG_LEN + 1];
};

class JobReqGameRoomList : public IJob
{
public:
	JobReqGameRoomList(uint64_t netId, uint32_t reqContextNo)
		: m_netId(netId)
		, m_reqContextNo(reqContextNo)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	uint32_t m_reqContextNo;
};

class JobReqCreateGameRoom : public IJob
{
public:
	JobReqCreateGameRoom(uint64_t netId, GameRoomTeamFormat tf, uint16_t roomNameLen, wchar_t* roomName);

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	GameRoomTeamFormat m_roomTeamFormat;
	uint16_t m_roomNameLen;
	wchar_t m_roomName[MAX_GAME_ROOM_NAME_LEN + 1];
};

class JobReqJoinGameRoom : public IJob
{
public:
	JobReqJoinGameRoom(uint64_t netId, uint64_t gameRoomId)
		: m_netId(netId)
		, m_gameRoomId(gameRoomId)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	uint64_t m_gameRoomId;
};

class JobReqExitGameRoom : public IJob
{
public:
	JobReqExitGameRoom(uint64_t netId)
		: m_netId(netId)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
};

class JobReqChangeTeam : public IJob
{
public:
	JobReqChangeTeam(uint64_t netId, GameTeam team)
		: m_netId(netId)
		, m_newTeam(team)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	GameTeam m_newTeam;
};

class JobReqChangeGameReadyState : public IJob
{
public:
	JobReqChangeGameReadyState(uint64_t netId, bool ready)
		: m_netId(netId)
		, m_ready(ready)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	bool m_ready;
};

class JobReqGameStart : public IJob
{
public:
	JobReqGameStart(uint64_t netId)
		: m_netId(netId)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
};

class JobSessionDisconnected : public IJob
{
public:
	JobSessionDisconnected(uint64_t netId)
		: m_netId(netId)
	{
	}

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
};

class LogicThread : public JobThreadBase<IJob>
{
public:
	LogicThread(winppy::TCPServer& server);
	virtual ~LogicThread() = default;
protected:
	virtual void ProcessJob(std::unique_ptr<IJob> upJob) override { upJob->Execute(*this); }
public:
	winppy::TCPServer& m_server;
	std::unordered_map<uint64_t, std::unique_ptr<Session>> m_sessions;
	std::unordered_map<uint32_t, std::unique_ptr<Player>> m_players;
	std::vector<Channel> m_channel;
};
