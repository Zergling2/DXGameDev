#pragma once

#include <ZergEngine\ZergEngine.h>
#include "GameResources.h"
#include "../Common/Constants.h"

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
	ze::UIObjectHandle m_hButtonOpenShop;
	ze::UIObjectHandle m_hButtonUserInfo;

	ze::UIObjectHandle m_hPanelGameListBrowserRoot;
	ze::UIObjectHandle m_hPanelGameSelectedIndicator;
	ze::UIObjectHandle m_hTextGameNo[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameName[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameMap[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameHeadcount[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameMode[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameState[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hButtonEnterGame[MAX_GAME_PER_LIST_PAGE];
};
