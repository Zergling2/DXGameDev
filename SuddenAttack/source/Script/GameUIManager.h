#pragma once

#include <ZergEngine\ZergEngine.h>
#include "Constants.h"
#include "Contents.h"

class GameUIManager;
class GameResources;
class Account;
class LobbyHandler;
class Player;
class ListenServerClient;

class IGameUIManagerState
{
public:
	virtual void Enter(GameUIManager* pGameUIManager) = 0;
	virtual void Update(GameUIManager* pGameUIManager, float dt) = 0;
	virtual void Exit(GameUIManager* pGameUIManager) = 0;
};

class GameUIStateDeactivate : public IGameUIManagerState
{
public:
	static GameUIStateDeactivate* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager, float dt);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static GameUIStateDeactivate s_instance;
};

class GameUIStatePlaying : public IGameUIManagerState
{
public:
	static GameUIStatePlaying* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager, float dt);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static GameUIStatePlaying s_instance;
};

class GameUIStateScoreboard : public IGameUIManagerState
{
public:
	static GameUIStateScoreboard* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager, float dt);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static GameUIStateScoreboard s_instance;
};

class GameUIStateMenu : public IGameUIManagerState
{
public:
	static GameUIStateMenu* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager, float dt);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static GameUIStateMenu s_instance;
};

class GameUIStateChatting : public IGameUIManagerState
{
public:
	static GameUIStateChatting* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager, float dt);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static GameUIStateChatting s_instance;
};

class GameUIManager : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	GameUIManager(ze::GameObject& owner);
	virtual ~GameUIManager() = default;

	virtual void Awake() override;
	// virtual void FixedUpdate() override;
	virtual void Update() override;

	void Init();

	void SetTextGameRemainingTime(float time);
	void SetTextHP(uint32_t hp);
	void SetTextAP(uint32_t ap);
	void SetTextAmmoState(const wchar_t* str);
	void ClearTextAmmoState();
	void SetTextPoint(uint32_t point);
	void SetTextWeaponName(const wchar_t* name);
	void ClearTextWeaponName();

	void SetState(IGameUIManagerState* pUIState);
	IGameUIManagerState* GetState() const { return m_pUIState; }

	void SetAccountScriptHandle(ze::ComponentHandle<Account> hScript) { m_hScriptAccount = hScript; }
	void SetGameResourcesScriptHandle(ze::ComponentHandle<GameResources> hScript) { m_hScriptGameResources = hScript; }
	void SetLobbyHandlerScriptHandle(ze::ComponentHandle<LobbyHandler> hScript) { m_hScriptLobbyHandler = hScript; }
	void SetPlayerScriptHandle(ze::ComponentHandle<Player> hScript) { m_hScriptPlayer = hScript; }
	void SetListenServerClientScriptHandle(ze::ComponentHandle<ListenServerClient> hScript) { m_hScriptListenServerClient = hScript; }
	Player* GetPlayerScript() const;
	
	void ShowAdapterInfo();
	void HideAdapterInfo();

	void ShowScoreboard();
	void HideScoreboard();

	void ShowMenu();
	void HideMenu();

	void ShowGameUI();
	void HideGameUI();

	void ShowChatPanel();
	void HideChatPanel();

	void SetScoreText(uint32_t redTeamScore, uint32_t blueTeamScore);
	void IncreaseScoreText(GameTeam team, uint32_t count);

	void AddKillLog(bool headShot, GameTeam killerTeam, const wchar_t* killerNickname, WeaponCode weapon, GameTeam deaderTeam, const wchar_t* deaderNickname);
	void RotateKillLog();

	void ClearAllChatMsgs();
	void __vectorcall AddChatMsg(const wchar_t* msg, XMVECTOR color = ColorsLinear::WhiteSmoke);
	void RotateChatMsg();
	void SendChatMsg();

	void StartRespawnUI(float time);

	void AddPlayer(uint32_t accountId, GameTeam team, uint16_t level, const wchar_t* nickname, uint32_t kill, uint32_t death, uint32_t ping);
	void RemovePlayer(uint32_t accountId);
	void SetPlayerKillDeath(uint32_t accountId, uint32_t kill, uint32_t death);
	void SetPlayerPing(uint32_t accountId, uint32_t ping);
	void SetPlayerHeadNicknameTextParent(ze::UIObjectHandle hText) const;
private:
	void OnPosChangePlayerFoV();
	void OnClickDrawDebugInfo();
	void OnClickWindowMode();
	void OnClickCloseGameMenu();
	void OnClickExitGame();
private:
	IGameUIManagerState* m_pUIState;
	bool m_activeRespawnUI;
	bool m_needUpdateChatMsgTransparency;
	float m_respawnRemainingTime;
	ze::ComponentHandle<Account> m_hScriptAccount;
	ze::ComponentHandle<GameResources> m_hScriptGameResources;
	ze::ComponentHandle<LobbyHandler> m_hScriptLobbyHandler;
	ze::ComponentHandle<Player> m_hScriptPlayer;
	ze::ComponentHandle<ListenServerClient> m_hScriptListenServerClient;

	ze::UIObjectHandle m_hPanelAdapterInfoRoot;

	ze::UIObjectHandle m_hPanelScoreboardRoot;
	ze::UIObjectHandle m_hTextScoreboardPlayerLevel[static_cast<size_t>(GameTeam::Count)][MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardPlayerNickname[static_cast<size_t>(GameTeam::Count)][MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardPlayerKillDeath[static_cast<size_t>(GameTeam::Count)][MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardPlayerPing[static_cast<size_t>(GameTeam::Count)][MAX_PLAYERS_PER_TEAM];
	std::vector<uint32_t> m_scoreboardPlayerAccountId[static_cast<size_t>(GameTeam::Count)];
	uint32_t m_score[static_cast<size_t>(GameTeam::Count)];

	ze::UIObjectHandle m_hPanelMenuRoot;
	ze::UIObjectHandle m_hSliderControlPlayerFoV;
	ze::UIObjectHandle m_hCheckboxDrawDebugInfo;
	ze::UIObjectHandle m_hCheckboxWindowMode;

	ze::UIObjectHandle m_hImageGameUIRoot;
	ze::UIObjectHandle m_hImageCrosshair;
	ze::UIObjectHandle m_hImageHPAPBackground;
	ze::UIObjectHandle m_hImageRBUIBackground;
	ze::UIObjectHandle m_hTextGameRemainingTime;
	ze::UIObjectHandle m_hTextRedTeamScore;
	ze::UIObjectHandle m_hTextTeamScoreGoal;
	ze::UIObjectHandle m_hTextBlueTeamScore;
	ze::UIObjectHandle m_hTextHP;
	ze::UIObjectHandle m_hTextAP;
	ze::UIObjectHandle m_hTextPoint;
	ze::UIObjectHandle m_hTextWeaponName;
	ze::UIObjectHandle m_hTextAmmoState;
	ze::UIObjectHandle m_hTextRespawnIndicator;
	static constexpr size_t MAX_KILL_LOG_ITEM_COUNT = 4;
	ze::UIObjectHandle m_hTextKillLogSpecial[MAX_KILL_LOG_ITEM_COUNT];
	ze::UIObjectHandle m_hTextKillLogKiller[MAX_KILL_LOG_ITEM_COUNT];
	ze::UIObjectHandle m_hImageKillLogWeapon[MAX_KILL_LOG_ITEM_COUNT];
	ze::UIObjectHandle m_hTextKillLogDeader[MAX_KILL_LOG_ITEM_COUNT];
	size_t m_killLogCount;
	
	ze::UIObjectHandle m_hPanelChatBackground;
	ze::UIObjectHandle m_hTextChatMsg[INGAME_CHAT_MSG_ITEM_ROW_COUNT];
	ze::UIObjectHandle m_hInputFieldChatMsg;
	size_t m_chatMsgCount;
	float m_chatMsgTransparencyTimer;
};
