#pragma once

#include <ZergEngine\ZergEngine.h>
#include <enet\enet.h>
#include "..\Resource\GamePlayerInfo.h"
#include "..\Resource\LSProtocol.h"

class GameResources;
class Network;
class Account;
class GameUIManager;
class Player;
class ThirdPersonCharacter;

class ListenServerClient : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	ListenServerClient(ze::GameObject& owner);
	virtual ~ListenServerClient() = default;

	virtual void FixedUpdate() override;
	virtual void OnDestroy() override;

	void OnConnect(ENetPeer* pPeer);
	void OnReceive(ENetPeer* pPeer, uint8_t channelId, const ENetPacket* pPacket);
	void OnDisconnect(ENetPeer* pPeer);

	void SetStartupInfo(uint32_t serverIP, uint16_t serverPort);
	void StartClient();
	void CloseClient();

	bool SendPacket(ENetPeer* pPeer, ENetPacket* pPacket) const;

	void SetGameResourcesScriptHandle(ze::ComponentHandle<GameResources> hScript) { m_hScriptGameResources = hScript; }
	void SetNetworkScriptHandle(ze::ComponentHandle<Network> hScript) { m_hScriptNetwork = hScript; }
	void SetAccountScriptHandle(ze::ComponentHandle<Account> hScript) { m_hScriptAccount = hScript; }
	void SetGameUIManagerScriptHandle(ze::ComponentHandle<GameUIManager> hScript) { m_hScriptGameUIManager = hScript; }
private:
	void OnSCResAuthResult(const LSSCResAuthResult* pPacket);
	void OnSCNotifyGameStatus(const LSSCNotifyGameStatus* pPacket);
	void OnSCNotifyChat(const LSSCNotifyChat* pPacket);
	void OnSCNotifyGamePlayerJoined(const LSSCNotifyGamePlayerJoined* pPacket);
	void OnSCNotifyGamePlayerExit(const LSSCNotifyGamePlayerExit* pPacket);
	void OnSCNotifyGamePlayerInfo(const LSSCNotifyGamePlayerInfo* pPacket);
	void OnSCNotifyGamePlayerTransform(const LSSCNotifyGamePlayerTransform* pPacket);
	void OnSCNotifyGamePlayerRespawn(const LSSCNotifyGamePlayerRespawn* pPacket);
private:
	uint32_t m_serverIP;
	uint16_t m_serverPort;
	ENetHost* m_pClient;
	ENetPeer* m_pPeer;		// host
	ze::ComponentHandle<GameResources> m_hScriptGameResources;
	ze::ComponentHandle<Network> m_hScriptNetwork;
	ze::ComponentHandle<Account> m_hScriptAccount;
	ze::ComponentHandle<GameUIManager> m_hScriptGameUIManager;
	ze::ComponentHandle<Player> m_hScriptPlayer;
	float m_gameRemainingTime;
	std::unordered_map<uint32_t, std::pair<std::unique_ptr<GamePlayerInfo>, ze::ComponentHandle<ThirdPersonCharacter>>> m_players;
};
