#pragma once

#include <unordered_map>
#include "JobThreadBase.h"
#include "Constants.h"
#include "GameRoom.h"
#include "Session.h"
#include "Player.h"

class Channel;
class Session;
class LogicThread;
class SAServer;

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
	virtual ~JobCreateNewSession() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
};

class JobReqLogin : public IJob
{
public:
	JobReqLogin(uint64_t netId, const wchar_t* id, const wchar_t* pw);
	virtual ~JobReqLogin() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_id[MAX_ID_LEN + 1];
	wchar_t m_pw[MAX_PW_LEN + 1];
};

class JobDBJobReqLoginResult : public IJob
{
public:
	JobDBJobReqLoginResult(uint64_t netId, bool querySuccess, bool result)
		: m_netId(netId)
		, m_querySuccess(querySuccess)
		, m_result(result)
		, m_accountId(0)
		, m_nicknameLen(0)
		, m_nickname{}
		, m_level(0)
		, m_exp(0)
		, m_point(0)
	{
	}
	virtual ~JobDBJobReqLoginResult() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	bool m_querySuccess;	// 쿼리 성공 여부
	bool m_result;			// 결과
public:
	uint32_t m_accountId;
	uint16_t m_nicknameLen;
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];
	uint16_t m_level;	// 레벨
	uint32_t m_exp;		// 경험치
	uint32_t m_point;	// 포인트 소유량
};

class JobReqCreateAccount : public IJob
{
public:
	JobReqCreateAccount(uint64_t netId, const wchar_t* id, const wchar_t* nickname, const wchar_t* pw);
	virtual ~JobReqCreateAccount() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_id[MAX_ID_LEN + 1];
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];
	wchar_t m_pw[MAX_PW_LEN + 1];
};

class JobDBJobCreateAccountResult : public IJob
{
public:
	JobDBJobCreateAccountResult(uint64_t netId, bool result)
		: m_netId(netId)
		, m_result(result)
	{
	}
	virtual ~JobDBJobCreateAccountResult() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	bool m_result;
};

class JobReqIdDuplicateCheck : public IJob
{
public:
	JobReqIdDuplicateCheck(uint64_t netId, const wchar_t* id);
	virtual ~JobReqIdDuplicateCheck() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_id[MAX_ID_LEN + 1];
};

class JobDBJobIdDuplicateCheckResult : public IJob
{
public:
	JobDBJobIdDuplicateCheckResult(uint64_t netId, bool querySuccess, bool duplicated)
		: m_netId(netId)
		, m_querySuccess(querySuccess)
		, m_duplicated(duplicated)
	{
	}
	virtual ~JobDBJobIdDuplicateCheckResult() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	bool m_querySuccess;		// 조회 성공 시 true, 실패 시 false
	bool m_duplicated;
};

class JobReqNicknameDuplicateCheck : public IJob
{
public:
	JobReqNicknameDuplicateCheck(uint64_t netId, const wchar_t* nickname);
	virtual ~JobReqNicknameDuplicateCheck() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];
};

class JobDBJobNicknameDuplicateCheckResult : public IJob
{
public:
	JobDBJobNicknameDuplicateCheckResult(uint64_t netId, bool querySuccess, bool duplicated)
		: m_netId(netId)
		, m_querySuccess(querySuccess)
		, m_duplicated(duplicated)
	{
	}
	virtual ~JobDBJobNicknameDuplicateCheckResult() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	bool m_querySuccess;		// 조회 성공 시 true, 실패 시 false
	bool m_duplicated;
};

class JobReqChannelInfo : public IJob
{
public:
	JobReqChannelInfo(uint64_t netId)
		: m_netId(netId)
	{
	}
	virtual ~JobReqChannelInfo() = default;

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
	virtual ~JobReqJoinChannel() = default;

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
	virtual ~JobReqExitChannel() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
};

class JobReqLobbyChat : public IJob
{
public:
	JobReqLobbyChat(uint64_t netId, uint16_t msgLen, const wchar_t* msg);
	virtual ~JobReqLobbyChat() = default;

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
	virtual ~JobReqGameRoomList() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
	uint32_t m_reqContextNo;
};

class JobReqCreateGameRoom : public IJob
{
public:
	JobReqCreateGameRoom(uint64_t netId, GameRoomTeamFormat tf, uint16_t roomNameLen, wchar_t* roomName);
	virtual ~JobReqCreateGameRoom() = default;

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
	virtual ~JobReqJoinGameRoom() = default;

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
	virtual ~JobReqExitGameRoom() = default;

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
	virtual ~JobReqChangeTeam() = default;

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
	virtual ~JobReqChangeGameReadyState() = default;

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
	virtual ~JobReqGameStart() = default;

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
	virtual ~JobSessionDisconnected() = default;

	virtual void Execute(LogicThread& thread) override;
private:
	uint64_t m_netId;
};

class LogicThread : public JobThreadBase<IJob>
{
public:
	LogicThread(SAServer& server);
	virtual ~LogicThread() = default;
protected:
	virtual void ProcessJob(std::unique_ptr<IJob> upJob) override { upJob->Execute(*this); }
public:
	SAServer& m_server;
	std::unordered_map<uint64_t, std::unique_ptr<Session>> m_sessions;
	std::unordered_map<uint32_t, std::unique_ptr<Player>> m_players;
	std::vector<std::unique_ptr<Channel>> m_channel;
};
