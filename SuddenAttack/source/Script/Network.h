#pragma once

#include <ZergEngine\ZergEngine.h>
#include <queue>
#include <winppy\Network\TCPClientEngine.h>
#include <winppy\Network\TCPClient.h>
#include <winppy\Network\Packet.h>
#include <enet\enet.h>

class Network;
class Account;
class LobbyHandler;

enum class ENetHostType
{
	None,
	Server,
	Client
};

class SAClient : public winppy::TCPClient
{
public:
	SAClient(Network& network)
		: m_network(network)
	{
	}
	virtual void OnConnect() override;
	virtual void OnReceive(winppy::Packet packet) override;
	virtual void OnDisconnect() override;
private:
	Network& m_network;
};

class Network : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
	friend class SAClient;
public:
	Network(ze::GameObject& owner);
	virtual ~Network() = default;

	SAClient& GetClient() { return m_client; }

	virtual void Awake() override;
	virtual void Update() override;
	virtual void OnDestroy() override;
private:
	void PktProcFromSAServer();

	void PktProcSCResLogin(winppy::Packet packet);
	void PktProcSCResIdDuplicateCheck(winppy::Packet packet);
	void PktProcSCResNicknameDuplicateCheck(winppy::Packet packet);
	void PktProcSCResCreateAccount(winppy::Packet packet);
	void PktProcSCResChannelInfo(winppy::Packet packet);
	void PktProcSCResJoinChannel(winppy::Packet packet);
	void PktProcSCResGameList(winppy::Packet packet);
	void PktProcSCResCreateGameRoom(winppy::Packet packet);
	void PktProcSCResJoinGameRoom(winppy::Packet packet);
	void PktProcSCResHostGameStartableState(winppy::Packet packet);
	void PktProcSCResExitGameRoom(winppy::Packet packet);
	void PktProcSCResExitGameChannel(winppy::Packet packet);
	void PktProcSCNotifyLobbyChat(winppy::Packet packet);
	void PktProcSCNotifyPlayerTeamChanged(winppy::Packet packet);
	void PktProcSCNotifyPlayerJoinedGameRoom(winppy::Packet packet);
	void PktProcSCNotifyPlayerExitGameRoom(winppy::Packet packet);
	void PktProcSCNotifyGameRoomPlayer(winppy::Packet packet);
	void PktProcSCNotifyHostChanged(winppy::Packet packet);
	void PktProcSCNotifyHostGameStarted(winppy::Packet packet);
	void PktProcSCNotifyPlayerStateChanged(winppy::Packet packet);
private:
	winppy::TCPClientEngine m_ce;
	SAClient m_client;
	SRWLOCK m_lock;	// m_packetQueue¿ë ¶ô
	std::queue<winppy::Packet> m_packetQueue;
	bool m_connected;
	bool m_disconnectJobDone;
	ENetHostType m_hostType;
	ENetHost* m_pHost;
public:
	ze::ComponentHandle<Account> m_hScriptAccount;
	ze::ComponentHandle<LobbyHandler> m_hScriptLobbyHandler;
};
