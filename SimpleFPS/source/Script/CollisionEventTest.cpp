#include "CollisionEventTest.h"

using namespace ze;

CollisionEventTest::CollisionEventTest(ze::GameObject& owner)
	: MonoBehaviour(owner)
{
}

void CollisionEventTest::Awake()
{
	if (Rigidbody* pRigidbody = m_hRigidbody.ToPtr())
		pRigidbody->ListenCollisionEvent();
}

void CollisionEventTest::OnTriggerEnter()
{
	printf("OnTriggerEnter\n");
}

void CollisionEventTest::OnTriggerStay()
{
	printf("OnTriggerStay\n");
}

void CollisionEventTest::OnTriggerExit()
{
	printf("OnTriggerExit\n");
}

void CollisionEventTest::OnCollisionEnter()
{
	printf("OnCollisionEnter\n");
}

void CollisionEventTest::OnCollisionStay()
{
	printf("OnCollisionStay\n");
}

void CollisionEventTest::OnCollisionExit()
{
	printf("OnCollisionExit\n");
}
