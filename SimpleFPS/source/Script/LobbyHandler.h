#pragma once

#include <ZergEngine\ZergEngine.h>

class LobbyHandler : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	LobbyHandler(ze::GameObject& owner);
	virtual ~LobbyHandler() = default;

	virtual void Awake() override;

	// UI Event Handlers
	void OnClickLogin();
	void OnClickExitGame();
public:
	bool m_connectionWithServer;
	ze::UIObjectHandle m_hImageLoginBackground;
	ze::UIObjectHandle m_hPanelLoginWindowRoot;
	ze::UIObjectHandle m_hInputFieldId;
	ze::UIObjectHandle m_hInputFieldPw;
};
