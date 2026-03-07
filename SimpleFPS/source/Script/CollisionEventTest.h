#pragma once

#include <ZergEngine\ZergEngine.h>

class CollisionEventTest : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	CollisionEventTest(ze::GameObject& owner);
	virtual ~CollisionEventTest() = default;

	virtual void Awake() override;

	virtual void OnTriggerEnter() override;
	virtual void OnTriggerStay() override;
	virtual void OnTriggerExit() override;
	virtual void OnCollisionEnter() override;
	virtual void OnCollisionStay() override;
	virtual void OnCollisionExit() override;
public:
	ze::ComponentHandle<ze::Rigidbody> m_hRigidbody;
};
