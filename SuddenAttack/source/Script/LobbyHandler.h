#pragma once

#include <ZergEngine\ZergEngine.h>
#include "Constants.h"
#include "Contents.h"

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

struct GameRoomItem
{
	uint64_t m_roomId;
	uint16_t m_gameNo;
	GameRoomMaxPlayer m_maxPlayer;
	uint8_t m_currPlayer;
	GameMap m_gameMap;
	GameMode m_gameMode;
	GameRoomState m_gameRoomState;
	wchar_t m_gameName[MAX_GAME_NAME_LEN + 1];
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
	void OnClickRefreshGameList();
	void OnClickGameListPrev();
	void OnClickGameListNext();
	void OnClickCreateGameRoom();
	void OnClickExitGameRoom();
	void OnClickCreateGameRoomCancel();
	void OnClickRadioButtonGameRoom1vs1();
	void OnClickRadioButtonGameRoom2vs2();
	void OnClickRadioButtonGameRoom3vs3();
	void OnClickRadioButtonGameRoom4vs4();
	void OnClickRadioButtonGameRoom5vs5();
	void OnClickRadioButtonGameRoom6vs6();
	void OnClickRadioButtonGameRoom7vs7();
	void OnClickRadioButtonGameRoom8vs8();

	// Logic
	void SetLobbyState(LobbyState state);
	void ClearChatMsgs();
	void AddChatMsg(const wchar_t* msg);
	void UpdateGameListBrowserUI();
private:
	void UpdateUI();
	uint32_t GetGameListPageCount() const;
	void ClearAllGameListItem();
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
	ze::UIObjectHandle m_hPanelChatRoot;
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
	ze::UIObjectHandle m_hTextGameListPage;

	ze::UIObjectHandle m_hPanelCreateGameRoomRoot;
	ze::UIObjectHandle m_hInputFieldCreateGameRoomName;
	ze::UIObjectHandle m_hRadioButtonGameRoomMaxPlayer[MAX_PLAYER_PER_TEAM];
	GameRoomMaxPlayer m_createGameRoomMaxPlayerSelected;

	ze::UIObjectHandle m_hPanelGameRoomRoot;
	ze::UIObjectHandle m_hTextGameRoomNamePanel;
	ze::UIObjectHandle m_hTextGameRoomRedTeamPlayer[MAX_PLAYER_PER_TEAM];
	ze::UIObjectHandle m_hTextGameRoomBlueTeamPlayer[MAX_PLAYER_PER_TEAM];

	std::vector<GameRoomItem> m_gameRoomList;
	uint32_t m_currGameListContextNo;	// 방목록 새로고침 컨텍스트
	uint32_t m_gameListReqContextNo;
	uint32_t m_currGameListPage;
};
