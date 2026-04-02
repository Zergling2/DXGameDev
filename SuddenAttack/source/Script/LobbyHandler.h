#pragma once

#include <ZergEngine\ZergEngine.h>
#include "Constants.h"
#include "Contents.h"

class GameResources;
class Network;

enum class LobbyState
{
	Login,
	ChannelListBrowser,
	GameListBrowser,
	GameRoom,
	Scoreboard,

	None
};

struct GameRoomItem
{
	uint64_t m_gameRoomId;
	uint16_t m_gameRoomNo;
	GameRoomMaxPlayer m_maxPlayer;
	uint8_t m_currPlayer;
	GameMap m_gameMap;
	GameMode m_gameMode;
	GameRoomState m_gameRoomState;
	wchar_t m_gameName[MAX_GAME_ROOM_NAME_LEN + 1];
};

struct GameRoomPlayer
{
	uint64_t m_netId;
	uint16_t m_level;	// 레벨
	wchar_t m_nickname[MAX_NICKNAME_LEN + 1];	// null termination 상태로 저장
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
	void OnClickJoinChannel0();
	void OnClickJoinChannel1();
	void OnClickJoinChannel2();
	void OnClickJoinChannel3();
	void OnClickJoinChannel4();
	void OnClickJoinChannel5();
	void OnClickRefreshChannelList();
	void OnClickSendChatMsg();
	void OnClickRefreshGameList();
	void OnClickGameListPrev();
	void OnClickGameListNext();
	void OnClickCreateGameRoom();
	void OnClickCreateGameRoomReq();
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
	void OnClickJoinGameRoom0();
	void OnClickJoinGameRoom1();
	void OnClickJoinGameRoom2();
	void OnClickJoinGameRoom3();
	void OnClickJoinGameRoom4();
	void OnClickJoinGameRoom5();
	void OnClickJoinGameRoom6();
	void OnClickJoinGameRoom7();
	void OnClickJoinGameRoom8();
	void OnClickJoinGameRoom9();
	void OnClickJoinGameRoom10();
	void OnClickJoinGameRoom11();
	void OnClickMoveToBlueTeam();
	void OnClickMoveToRedTeam();

	void OnClickOkMsgBoxOk();

	// Logic
	void SetLobbyState(LobbyState state);
	void ClearChatMsgs();
	void ClearGameRoomInfo();
	void SetGameRoomInfo(uint64_t gameRoomId, uint64_t hostNetId, GameRoomMaxPlayer maxPlayer, GameMap gameMap, GameMode gameMode, const wchar_t* gameRoomHeadText, GameTeam myTeam);
	void AddGameRoomPlayerInfo(GameTeam team, uint64_t netId, uint16_t level, const wchar_t* nickname);
	void MoveGameRoomPlayerInfo(uint64_t netId, GameTeam newTeam);
	void RemoveGameRoomPlayerInfo(uint64_t netId);
	void AddChatMsg(const wchar_t* msg);
	void UpdateGameListBrowserUI();
private:
	void UpdateUI();
	uint32_t GetGameListPageCount() const;
	void ClearAllGameListItem();

	void SendJoinChannelPacket(uint16_t channelId);
	void OnClickJoinGameRoomImpl(size_t index);
private:
	bool m_needUIUpdate;
	LobbyState m_lobbyState;
	size_t m_textLobbyChatMsgCount;
public:
	// 스크립트 관련
	ze::ComponentHandle<GameResources> m_hScriptGameResources;
	ze::ComponentHandle<Network> m_hScriptNetwork;

	// 바탕화면 UI 관련
	ze::UIObjectHandle m_hImageLobbyBgr;

	// 로그인 UI 관련
	ze::UIObjectHandle m_hPanelLoginUIRoot;
	ze::UIObjectHandle m_hInputFieldId;
	ze::UIObjectHandle m_hInputFieldPw;
	ze::UIObjectHandle m_hTextIdPwInputFieldHelpMsg;

	// 공통 버튼 UI 관련
	ze::UIObjectHandle m_hButtonExitGame;
	ze::UIObjectHandle m_hButtonOpenShop;
	ze::UIObjectHandle m_hButtonUserInfo;

	// '확인' 메시지 박스 UI 관련
	ze::UIObjectHandle m_hPanelOkMsgBoxRoot;
	ze::UIObjectHandle m_hTextOkMsgBoxMsg;
	ze::UIObjectHandle m_hPanelChannelBrowserRoot;
	ze::UIObjectHandle m_hTextChannelHeadcount[CHANNEL_COUNT];

	// 채팅 UI 관련
	ze::UIObjectHandle m_hPanelChatRoot;
	ze::UIObjectHandle m_hInputFieldChatMsg;
	ze::UIObjectHandle m_hTextLobbyChatMsg[CHAT_MSG_ITEM_ROW_COUNT];

	// 게임 브라우저 UI 관련
	ze::UIObjectHandle m_hPanelGameListBrowserRoot;
	ze::UIObjectHandle m_hPanelGameSelectedIndicator;
	ze::UIObjectHandle m_hTextGameNo[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameName[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameMap[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameHeadcount[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameMode[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameState[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hButtonJoinGameRoom[MAX_GAME_PER_LIST_PAGE];
	ze::UIObjectHandle m_hTextGameListPage;

	// 게임 방 생성 UI 관련
	ze::UIObjectHandle m_hPanelCreateGameRoomRoot;
	ze::UIObjectHandle m_hInputFieldCreateGameRoomName;
	ze::UIObjectHandle m_hRadioButtonGameRoomMaxPlayer[MAX_PLAYERS_PER_TEAM];
	GameRoomMaxPlayer m_createGameRoomMaxPlayerSelected;

	// 게임 방 관련
	uint64_t m_gameRoomId;	// 현재 입장해있는 게임 방 식별자
	uint64_t m_gameRoomHostNetId;
	GameRoomMaxPlayer m_gameRoomMaxPlayer;
	GameMap m_gameRoomGameMap;
	GameMode m_gameRoomGameMode;
	GameTeam m_gameRoomMyTeam;
	GameRoomPlayer m_gameRoomRedTeamPlayers[MAX_PLAYERS_PER_TEAM];
	GameRoomPlayer m_gameRoomBlueTeamPlayers[MAX_PLAYERS_PER_TEAM];
	uint8_t m_gameRoomRedTeamPlayersCount;
	uint8_t m_gameRoomBlueTeamPlayersCount;
	ze::UIObjectHandle m_hPanelGameRoomRoot;
	ze::UIObjectHandle m_hTextGameRoomNamePanel;
	ze::UIObjectHandle m_hTextGameRoomRedTeamPlayers[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextGameRoomBlueTeamPlayers[MAX_PLAYERS_PER_TEAM];


	// 게임 리스트 컨텍스트 관련
	std::vector<GameRoomItem> m_gameRoomList;
	uint32_t m_currGameListContextNo;	// 방목록 새로고침 컨텍스트 (네트워크 라이브러리의 패킷 송신 크기 제한으로 인해 필요)
	uint32_t m_gameListReqContextNo;
	uint32_t m_currGameListPage;
};
