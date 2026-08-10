#pragma once

#include <ZergEngine\ZergEngine.h>
#include <enet\enet.h>
#include "Contents.h"

class Network;
class GameUIManager;

class ListenServer : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	ListenServer(ze::GameObject& owner);
	virtual ~ListenServer() = default;

	virtual void Awake() override;
	virtual void Update() override;

	void SetStartInfo(GameTeam startingTeam, GameMap startingMap);
	void StartServer();
	void CloseServer();

	void SetNetworkScriptHandle(ze::ComponentHandle<Network> hScript) { m_hScriptNetwork = hScript; }
	void SetGameUIManagerScriptHandle(ze::ComponentHandle<GameUIManager> hScript) { m_hScriptGameUIManager = hScript; }
private:
	void ReleaseENetHost();
private:
	ze::ComponentHandle<Network> m_hScriptNetwork;
	ze::ComponentHandle<GameUIManager> m_hScriptGameUIManager;
	ENetHost* m_pHost;
	GameTeam m_startingTeam;
	GameMap m_startingMap;
};
