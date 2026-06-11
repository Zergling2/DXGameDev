#pragma once

#include <ZergEngine\ZergEngine.h>
#include "Constants.h"
#include "Contents.h"

class GameResources;
class Network;
class Account;

enum class LobbyState
{
	Login,
	ChannelListBrowser,
	GameListBrowser,
	GameRoom,
	Scoreboard,

	None
};

struct ChannelInfo;

struct GameRoomItem
{
	uint64_t m_id;
	uint16_t m_no;
	GameRoomTeamFormat m_tf;
	uint8_t m_numOfPlayers;
	GameMap m_map;
	GameRoomState m_state;
	wchar_t m_name[MAX_GAME_ROOM_NAME_LEN + 1];
};

struct GameRoomPlayer
{
	uint32_t m_accountId;
	uint16_t m_level;
	std::wstring m_nickname;
	PlayerState m_state;
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
	void OnClickCreateAccount();
	void OnClickIdDuplicateCheck();
	void OnClickNicknameDuplicateCheck();
	void OnClickRequestCreateAccount();
	void OnClickCancelCreateAccount();
	void OnClickExitGame();
	void OnClickExitGameChannel();
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
	void OnClickHostGameStart();
	void OnClickGameReady();
	void OnClickGameUnready();
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
	bool FindGameRoomPlayer(uint32_t accountId, GameTeam& team, size_t& index) const;
	const GameRoomPlayer* FindMyGameRoomPlayer() const;
	void SetLobbyState(LobbyState state);
	void ClearChatMsgs();
	void UpdateGameRoomUI();
	void OnReceiveChannelInfo(const ChannelInfo* pInfos, size_t count);
	void OnReceiveGameList(uint32_t listContextNo, const std::vector<GameRoomItem>& list);
	void OnJoinGameRoom(uint32_t hostAccountId, uint16_t roomNo, GameRoomTeamFormat tf, GameMap map, GameTeam team, const wchar_t* gameRoomName);	// 방에 입장했을 때
	void OnPlayerJoinGameRoom(uint32_t accountId, uint16_t level, wchar_t* nickname, PlayerState state, GameTeam team);	// 다른 플레이어가 방에 들어왔을 때
	void OnPlayerTeamChanged(uint32_t accountId, GameTeam newTeam);
	void OnExitGameRoom();
	void OnPlayerExitGameRoom(uint32_t accountId);
	void OnGameRoomHostChanged(uint32_t newHostAccountId);
	void OnGameRoomPlayerStateChanged(uint32_t accountId, PlayerState state);
	void AddChatMsg(const wchar_t* msg);
	void UpdateGameListBrowserUI();
	void ShowSelectedGameRoomIndicator(bool b) { m_showSelectedGameRoomIndicator = b; }
private:
	void UpdateUI();
	uint32_t GetGameListPageCount() const;
	void ClearAllGameListItem();

	void SendJoinChannelPacket(uint8_t channelId);
	void OnClickJoinGameRoomImpl(size_t index);
private:
	bool m_needUIUpdate;
	bool m_showSelectedGameRoomIndicator;
	LobbyState m_lobbyState;
	size_t m_textLobbyChatMsgCount;
public:
	// 스크립트 관련
	ze::ComponentHandle<GameResources> m_hScriptGameResources;
	ze::ComponentHandle<Network> m_hScriptNetwork;
	ze::ComponentHandle<Account> m_hScriptAccount;

	// 바탕화면 UI 관련
	ze::UIObjectHandle m_hImageLobbyBgr;

	// 로그인 UI 관련
	ze::UIObjectHandle m_hPanelLoginWindowRoot;
	ze::UIObjectHandle m_hInputFieldLoginId;
	ze::UIObjectHandle m_hInputFieldLoginPw;
	ze::UIObjectHandle m_hTextLoginHelpMsg;

	// 계정 생성 UI 관련
	ze::UIObjectHandle m_hPanelCreateAccountWindowRoot;
	ze::UIObjectHandle m_hInputFieldCreateAccountId;
	ze::UIObjectHandle m_hTextCreateAccountIdDuplicateCheckMsg;
	ze::UIObjectHandle m_hInputFieldCreateAccountPw;
	ze::UIObjectHandle m_hInputFieldCreateAccountPwDoubleCheck;
	ze::UIObjectHandle m_hTextCreateAccountPwCheckMsg;
	ze::UIObjectHandle m_hInputFieldCreateAccountNickname;
	ze::UIObjectHandle m_hTextCreateAccountNicknameDuplicateCheckMsg;

	// 공통 버튼 UI 관련
	ze::UIObjectHandle m_hButtonOpenShop;
	ze::UIObjectHandle m_hButtonUserInfo;

	// 'OK' 메시지 박스 UI 관련
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
	ze::UIObjectHandle m_hRadioButtonGameRoomTeamFormat[static_cast<size_t>(GameRoomTeamFormat::Count)];
	GameRoomTeamFormat m_createGameRoomTeamFormatSelected;

	// 게임 방 관련
	uint32_t m_gameRoomHostAccountId;
	uint16_t m_gameRoomNo;
	std::wstring m_gameRoomName;
	GameRoomTeamFormat m_gameRoomTeamFormat;
	GameMap m_gameRoomGameMap;
	GameTeam m_gameRoomMyTeam;
	std::vector<GameRoomPlayer> m_gameRoomRedTeamPlayers;
	std::vector<GameRoomPlayer> m_gameRoomBlueTeamPlayers;
	// 게임 방 UI
	ze::UIObjectHandle m_hPanelGameRoomRoot;
	ze::UIObjectHandle m_hImageGameRoomMapPreview;
	ze::UIObjectHandle m_hTextGameRoomInfo;
	ze::UIObjectHandle m_hTextGameRoomNamePanel;
	ze::UIObjectHandle m_hTextGameRoomRedTeamPlayers[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextGameRoomBlueTeamPlayers[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hButtonHostGameStart;
	ze::UIObjectHandle m_hButtonGameReady;
	ze::UIObjectHandle m_hButtonGameUnready;


	// 게임 리스트 컨텍스트 관련
	std::vector<GameRoomItem> m_gameRoomList;
	uint32_t m_currGameListContextNo;	// 방목록 새로고침 컨텍스트 (네트워크 라이브러리의 패킷 송신 크기 제한으로 인해 필요)
	uint32_t m_gameListReqContextNo;
	uint32_t m_currGameListPage;
};
