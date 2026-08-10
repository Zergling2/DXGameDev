#pragma once

#include <ZergEngine\ZergEngine.h>
#include <queue>
#include <winppy\Network\TCPClientEngine.h>
#include <winppy\Network\TCPClient.h>
#include <winppy\Network\Packet.h>

class Network;
class Account;
class LobbyHandler;
class ListenServer;

class SAClient : public winppy::TCPClient
{
public:
	SAClient();

	virtual void OnConnect() override;
	virtual void OnReceive(winppy::Packet packet) override;
	virtual void OnDisconnect() override;

	bool IsConnected() const { return m_connected; }
	bool DisconnectJobDone() const { return m_disconnectJobDone; }
public:
	SRWLOCK m_lock;	// m_packetQueue¿ë ¶ô
	std::queue<winppy::Packet> m_packetQueue;
private:
	bool m_connected;
	bool m_disconnectJobDone;
};

class Network : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	Network(ze::GameObject& owner);
	virtual ~Network() = default;

	SAClient& GetClient() { return m_client; }

	void SetLobbyHandlerScriptHandle(ze::ComponentHandle<LobbyHandler> hScript) { m_hScriptLobbyHandler = hScript; }
	void SetAccountScriptHandle(ze::ComponentHandle<Account> hScript) { m_hScriptAccount = hScript; }
	void SetListenServerScriptHandle(ze::ComponentHandle<ListenServer> hScript) { m_hScriptListenServer = hScript; }
	

	virtual void Awake() override;
	virtual void FixedUpdate() override;
	virtual void OnDestroy() override;
private:
	void PktProcSCResLogin(winppy::Packet packet);
	void PktProcSCResIdDuplicateCheck(winppy::Packet packet);
	void PktProcSCResNicknameDuplicateCheck(winppy::Packet packet);
	void PktProcSCResCreateAccount(winppy::Packet packet);
	void PktProcSCResChannelInfo(winppy::Packet packet);
	void PktProcSCResJoinChannel(winppy::Packet packet);
	void PktProcSCResGameList(winppy::Packet packet);
	void PktProcSCResCreateGameRoom(winppy::Packet packet);
	void PktProcSCResJoinGameRoom(winppy::Packet packet);
	void PktProcSCResHostGameStart(winppy::Packet packet);
	void PktProcSCResExitGameRoom(winppy::Packet packet);
	void PktProcSCResExitGameChannel(winppy::Packet packet);
	void PktProcSCNotifyLobbyChat(winppy::Packet packet);
	void PktProcSCNotifyPlayerTeamChanged(winppy::Packet packet);
	void PktProcSCNotifyPlayerJoinedGameRoom(winppy::Packet packet);
	void PktProcSCNotifyPlayerExitGameRoom(winppy::Packet packet);
	void PktProcSCNotifyGameRoomPlayer(winppy::Packet packet);
	void PktProcSCNotifyGameRoomStateChanged(winppy::Packet packet);
	void PktProcSCNotifyHostChanged(winppy::Packet packet);
	void PktProcSCNotifyPlayerStateChanged(winppy::Packet packet);
private:
	ze::ComponentHandle<LobbyHandler> m_hScriptLobbyHandler;
	ze::ComponentHandle<Account> m_hScriptAccount;
	ze::ComponentHandle<ListenServer> m_hScriptListenServer;

	winppy::TCPClientEngine m_ce;
	SAClient m_client;
};
