#pragma once

#include <ZergEngine\ZergEngine.h>

class FirstPersonController : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	FirstPersonController(ze::GameObject& owner);
	virtual ~FirstPersonController() = default;

	virtual void Awake() override;
	virtual void Update() override;

	void TestHandlerOnClick01();
	void TestHandlerOnClick02();
	void TestHandlerOnClick03();
};
