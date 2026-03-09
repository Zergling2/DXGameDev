#pragma once

#include <ZergEngine\ZergEngine.h>
#include "GameResources.h"

enum class LobbyState
{
	Login,
	GameListBrowser,
	GameRoom,
	Scoreboard
};

class LobbyHandler : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	LobbyHandler(ze::GameObject& owner);
	virtual ~LobbyHandler() = default;

	virtual void Awake() override;
	virtual void Update();

	// UI Event Handlers
	void OnClickLogin();
	void OnClickExitGame();

	// Logic
	void RequestUIUpdate() { m_uiUpdateReq = true; }
	void SetLobbyState(LobbyState state) { m_lobbyState = state; }

private:
	void UpdateUI();
private:
	bool m_connectionWithServer;
	bool m_uiUpdateReq;
	LobbyState m_lobbyState;
public:
	ze::ComponentHandle<GameResources> m_hScriptGameResources;

	ze::UIObjectHandle m_hImageLobbyBgr;
	ze::UIObjectHandle m_hPanelLoginUIRoot;
	ze::UIObjectHandle m_hInputFieldId;
	ze::UIObjectHandle m_hInputFieldPw;
	ze::UIObjectHandle m_hPanelGameListBrowserRoot;
};
