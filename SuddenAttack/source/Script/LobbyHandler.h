#pragma once

#include <ZergEngine\ZergEngine.h>
#include "Constants.h"

class GameResources;
class Network;

enum class LobbyState
{
	Login,
	GameListBrowser,
	GameRoom,
	Scoreboard,

	None
};

class LobbyHandler : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	LobbyHandler(ze::GameObject& owner);
	virtual ~LobbyHandler() = default;

	virtual void Awake() override;
	virtual void Start() override;
	virtual void Update();

	// UI Event Handlers
	void OnClickLogin();
	void OnClickExitGame();
	void OnClickSendChatMsg();

	// Logic
	void SetLobbyState(LobbyState state);
	void ClearChatMsgs();
	void AddChatMsg(const wchar_t* msg);
private:
	void UpdateUI();
private:
	bool m_needUIUpdate;
	LobbyState m_lobbyState;
	size_t m_textLobbyChatMsgCount;
public:
	ze::ComponentHandle<GameResources> m_hScriptGameResources;
	ze::ComponentHandle<Network> m_hScriptNetwork;

	ze::UIObjectHandle m_hImageLobbyBgr;
	ze::UIObjectHandle m_hPanelLoginUIRoot;
	ze::UIObjectHandle m_hInputFieldId;
	ze::UIObjectHandle m_hInputFieldPw;
	ze::UIObjectHandle m_hTextIdPwInputFieldHelpMsg;
	ze::UIObjectHandle m_hPanelChat;
	ze::UIObjectHandle m_hInputFieldChatMsg;
	ze::UIObjectHandle m_hTextLobbyChatMsg[CHAT_MSG_ITEM_ROW_COUNT];
	ze::UIObjectHandle m_hButtonOpenShop;
	ze::UIObjectHandle m_hButtonUserInfo;

	ze::UIObjectHandle m_hPanelGameBrowserRoot;
	ze::UIObjectHandle m_hPanelGameSelectedIndicator;
	ze::UIObjectHandle m_hTextGameNo[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameName[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameMap[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameHeadcount[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameMode[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameState[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hButtonEnterGame[MAX_GAME_PER_LIST_PAGE];
};
