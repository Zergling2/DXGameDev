#pragma once

#include <ZergEngine\ZergEngine.h>

class UIHandler : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	UIHandler(ze::GameObject& owner);
	virtual ~UIHandler() = default;

	// virtual void Awake() override;
	// virtual void FixedUpdate() override;
	// virtual void Update() override;
public:
	ze::UIObjectHandle m_hImageCrosshair;
	ze::UIObjectHandle m_hImageHealthBackground;
	ze::UIObjectHandle m_hImageRBUIBackground;
	ze::UIObjectHandle m_hTextHP;
	ze::UIObjectHandle m_hTextAP;
	ze::UIObjectHandle m_hTextPoint;
	ze::UIObjectHandle m_hTextWeaponName;
	ze::UIObjectHandle m_hTextAmmo;
	ze::UIObjectHandle m_h;
};
