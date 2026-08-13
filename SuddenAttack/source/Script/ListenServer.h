#pragma once

#include <ZergEngine\ZergEngine.h>
#include <enet\enet.h>
#include "Contents.h"
#include "..\Resource\GamePlayer.h"

class Network;
class GameUIManager;
class GamePlayer;

class ListenServer : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	ListenServer(ze::GameObject& owner);
	virtual ~ListenServer() = default;

	virtual void Awake() override;
	virtual void Update() override;
	virtual void OnDestroy() override;

	void SetStartupInfo(GameMap startingMap, const uint32_t* pStartingPlayersAccountIds, const GameTeam* pStartingPlayersTeam, size_t count);
	void StartServer();
	void CloseServer();

	bool IsReady() const { return m_ready; }
	void InitState();

	void SetNetworkScriptHandle(ze::ComponentHandle<Network> hScript) { m_hScriptNetwork = hScript; }
	void SetGameUIManagerScriptHandle(ze::ComponentHandle<GameUIManager> hScript) { m_hScriptGameUIManager = hScript; }
private:
	void CreateMainPlayer();
private:
	ze::ComponentHandle<Network> m_hScriptNetwork;
	ze::ComponentHandle<GameUIManager> m_hScriptGameUIManager;
	ENetHost* m_pHost;
	GameMap m_map;
	std::map<uint32_t, GameTeam> m_playersTeam;
	std::vector<std::unique_ptr<GamePlayer>> m_redTeamPlayers;
	std::vector<std::unique_ptr<GamePlayer>> m_blueTeamPlayers;
	bool m_ready;
};
