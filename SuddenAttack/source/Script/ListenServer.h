#pragma once

#include <ZergEngine\ZergEngine.h>
#include <enet\enet.h>
#include "Contents.h"
#include "..\Resource\LSProtocol.h"

class Network;
struct LSGamePlayerInfo;

class ListenServer : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	ListenServer(ze::GameObject& owner);
	virtual ~ListenServer() = default;

	virtual void Awake() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;
	virtual void OnDestroy() override;

	void OnConnect(ENetPeer* pPeer);
	void OnReceive(ENetPeer* pPeer, uint8_t channelId, const ENetPacket* pPacket);
	void OnDisconnect(ENetPeer* pPeer);

	void StartServer(GameMap map, float gameDuration, const uint32_t* pStartingPlayersAccountIds, const GameTeam* pStartingPlayersTeam, size_t count);
	void CloseServer();

	void SetNetworkScriptHandle(ze::ComponentHandle<Network> hScript) { m_hScriptNetwork = hScript; }

	bool SendPacket(ENetPeer* pPeer, ENetPacket* pPacket) const;
	size_t BroadcastPacket(ENetPacket* pPacket) const;
	size_t BroadcastPacketExcept(ENetPacket* pPacket, const ENetPeer* pExceptor) const;
private:
	void UpdateRespawn(float dt, LSGamePlayerInfo& player);

	void OnCSReqAuth(const LSCSReqAuth* pPacket, ENetPeer* pRequester);
	void OnCSReqChat(const LSCSReqChat* pPacket, ENetPeer* pRequester);
	void OnCSNotifyGamePlayerWeaponEvent(const LSCSNotifyGamePlayerWeaponEvent* pPacket, ENetPeer* pRequester);
	void OnCSNotifyGamePlayerTransform(const LSCSNotifyGamePlayerTransform* pPacket, ENetPeer* pRequester);
	void OnCSNotifyGamePlayerHit(const LSCSNotifyGamePlayerHit* pPacket, ENetPeer* pRequester);
private:
	ze::ComponentHandle<Network> m_hScriptNetwork;
	ENetHost* m_pHost;
	std::unordered_map<ENetPeer*, std::shared_ptr<LSGamePlayerInfo>> m_peers;
	std::unordered_map<uint32_t, std::pair<std::shared_ptr<LSGamePlayerInfo>, ENetPeer*>> m_peersWithAccountId;
	GameMap m_map;
	float m_gameRemainingTime;
	std::unordered_map<uint32_t, GameTeam> m_playersTeam;
};
