#pragma once

#include <ZergEngine\ZergEngine.h>
#include <Constants.h>

class GameUIManager;
class Player;

class IGameUIManagerState
{
public:
	virtual void Enter(GameUIManager* pGameUIManager) = 0;
	virtual void Update(GameUIManager* pGameUIManager) = 0;
	virtual void Exit(GameUIManager* pGameUIManager) = 0;
};

class GameUIStateDeactivate : public IGameUIManagerState
{
public:
	static GameUIStateDeactivate* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static GameUIStateDeactivate s_instance;
};

class GameUIStatePlaying : public IGameUIManagerState
{
public:
	static GameUIStatePlaying* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static GameUIStatePlaying s_instance;
};

class GameUIStateScoreboard : public IGameUIManagerState
{
public:
	static GameUIStateScoreboard* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static GameUIStateScoreboard s_instance;
};

class GameUIStateMenu : public IGameUIManagerState
{
public:
	static GameUIStateMenu* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static GameUIStateMenu s_instance;
};

class GameUIStateChatting : public IGameUIManagerState
{
public:
	static GameUIStateChatting* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static GameUIStateChatting s_instance;
};

struct PlayerInfo
{
	uint32_t m_level;
	std::wstring m_nickname;
	uint32_t m_kill;
	uint32_t m_death;
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
	virtual void LateUpdate() override;

	void SetTextHP(uint32_t hp);
	void SetTextAP(uint32_t ap);
	void SetTextAmmoState(const wchar_t* str);
	void ClearTextAmmoState();
	void SetTextPoint(uint32_t point);
	void SetTextWeaponName(const wchar_t* name);
	void ClearTextWeaponName();

	void SetState(IGameUIManagerState* pUIState);
	IGameUIManagerState* GetState() const { return m_pUIState; }

	void SetPlayerScriptHandle(ze::ComponentHandle<Player> hScriptPlayer) { m_hScriptPlayer = hScriptPlayer; }
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

	void OnClickCloseGameMenu();
	void ClearAllChatMsgs();
	void SendChatMsg();
private:
	IGameUIManagerState* m_pUIState;
	ze::ComponentHandle<Player> m_hScriptPlayer;

	ze::UIObjectHandle m_hPanelAdapterInfoRoot;

	ze::UIObjectHandle m_hPanelScoreboardRoot;
	ze::UIObjectHandle m_hTextScoreboardRedTeamPlayerLevel[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardRedTeamPlayerNickname[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardRedTeamPlayerKill[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardRedTeamPlayerDeath[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardRedTeamPlayerLatency[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardBlueTeamPlayerLevel[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardBlueTeamPlayerNickname[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardBlueTeamPlayerKill[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardBlueTeamPlayerDeath[MAX_PLAYERS_PER_TEAM];
	ze::UIObjectHandle m_hTextScoreboardBlueTeamPlayerLatency[MAX_PLAYERS_PER_TEAM];
	uint32_t m_redTeamPlayersCount;
	uint32_t m_blueTeamPlayersCount;
	uint64_t m_scoreboardRedTeamPlayersNetId[MAX_PLAYERS_PER_TEAM];
	uint64_t m_scoreboardBlueTeamPlayersNetId[MAX_PLAYERS_PER_TEAM];


	ze::UIObjectHandle m_hPanelMenuRoot;


	ze::UIObjectHandle m_hImageGameUIRoot;
	ze::UIObjectHandle m_hImageCrosshair;
	ze::UIObjectHandle m_hImageHealthBackground;
	ze::UIObjectHandle m_hImageRBUIBackground;
	ze::UIObjectHandle m_hTextHP;
	ze::UIObjectHandle m_hTextAP;
	ze::UIObjectHandle m_hTextPoint;
	ze::UIObjectHandle m_hTextWeaponName;
	ze::UIObjectHandle m_hTextAmmoState;


	ze::UIObjectHandle m_hPanelChatRoot;
	ze::UIObjectHandle m_hTextChatMsg[INGAME_CHAT_MSG_ITEM_ROW_COUNT];
	ze::UIObjectHandle m_hInputFieldChatMsg;
};
