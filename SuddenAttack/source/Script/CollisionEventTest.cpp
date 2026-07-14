#include "CollisionEventTest.h"

using namespace ze;

CollisionEventTest::CollisionEventTest(ze::GameObject& owner)
	: MonoBehaviour(owner)
{
}

void CollisionEventTest::Awake()
{
	if (Rigidbody* pRigidbody = m_hRigidbodyCollider.ToPtr())
		pRigidbody->ListenCollisionEvent();
}

void CollisionEventTest::OnTriggerEnter()
{
	printf("%s OnTriggerEnter\n", m_str.c_str());
}

void CollisionEventTest::OnTriggerStay()
{
	printf("%s OnTriggerStay\n", m_str.c_str());
}

void CollisionEventTest::OnTriggerExit()
{
	printf("%s OnTriggerExit\n", m_str.c_str());
}

void CollisionEventTest::OnCollisionEnter()
{
	printf("%s OnCollisionEnter\n", m_str.c_str());
}

void CollisionEventTest::OnCollisionStay()
{
	printf("%s OnCollisionStay\n", m_str.c_str());
}

void CollisionEventTest::OnCollisionExit()
{
	printf("%s OnCollisionExit\n", m_str.c_str());
}
