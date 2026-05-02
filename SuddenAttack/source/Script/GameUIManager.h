#pragma once

#include <ZergEngine\ZergEngine.h>
#include <Constants.h>

class GameUIManager;
class PlayerController;

class IUIState
{
public:
	virtual void Enter(GameUIManager* pGameUIManager) = 0;
	virtual void Update(GameUIManager* pGameUIManager) = 0;
	virtual void Exit(GameUIManager* pGameUIManager) = 0;
};

class UIStateNone : public IUIState
{
public:
	static UIStateNone* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static UIStateNone s_instance;
};

class UIStateScoreboard : public IUIState
{
public:
	static UIStateScoreboard* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static UIStateScoreboard s_instance;
};

class UIStateGameMenu : public IUIState
{
public:
	static UIStateGameMenu* GetState() { return &s_instance; }

	virtual void Enter(GameUIManager* pGameUIManager);
	virtual void Update(GameUIManager* pGameUIManager);
	virtual void Exit(GameUIManager* pGameUIManager);
private:
	static UIStateGameMenu s_instance;
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

	void SetHP(uint32_t hp) { m_hp = hp; }
	void SetAP(uint32_t ap) { m_ap = ap; }
	void UpdateTextHP();
	void UpdateTextAP();

	void SetAmmoInMag(uint32_t ammoInMag) { m_ammoInMag = ammoInMag; }
	void SetAmmoInReserve(uint32_t ammoInReserve) { m_ammoInReserve = ammoInReserve; }
	void UpdateTextAmmo();

	void SetPoint(uint32_t point) { m_point = point; }
	void UpdateTextPoint();

	void SetCurrWeaponName(std::wstring name) { m_currWeaponName = std::move(name); }
	void UpdateTextWeaponName();

	void SetState(IUIState* pUIState);
	IUIState* GetState() const { return m_pUIState; }


	void OnClickCloseGameMenu();
private:
	IUIState* m_pUIState;
public:
	ze::UIObjectHandle m_hImageCrosshair;
	ze::UIObjectHandle m_hImageHealthBackground;
	ze::UIObjectHandle m_hImageRBUIBackground;
	ze::UIObjectHandle m_hTextHP;
	ze::UIObjectHandle m_hTextAP;
	ze::UIObjectHandle m_hTextPoint;
	ze::UIObjectHandle m_hTextWeaponName;
	ze::UIObjectHandle m_hTextAmmo;

	ze::UIObjectHandle m_hPanelGameMenuRoot;
	ze::UIObjectHandle m_hPanelScoreboardRoot;
	uint32_t m_redTeamPlayersCount;
	uint32_t m_blueTeamPlayersCount;
	uint64_t m_scoreboardRedTeamPlayersNetId[MAX_PLAYERS_PER_TEAM];
	uint64_t m_scoreboardBlueTeamPlayersNetId[MAX_PLAYERS_PER_TEAM];
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

	ze::ComponentHandle<PlayerController> m_hScriptPlayerController;

	uint32_t m_hp;
	uint32_t m_ap;
	uint32_t m_ammoInMag;
	uint32_t m_ammoInReserve;
	uint32_t m_point;
	std::wstring m_currWeaponName;
};
