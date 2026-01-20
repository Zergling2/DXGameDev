#include <ZergEngine\CoreSystem\Resource\CapsuleCollider.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <bullet3\BulletCollision\CollisionShapes\btCapsuleShape.h>

using namespace ze;

CapsuleCollider::CapsuleCollider(float radius, float height)
{
	m_upBtCollisionShape = std::make_unique<btCapsuleShape>(radius, height);
}
