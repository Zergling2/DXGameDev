#include <ZergEngine\CoreSystem\Resource\SphereCollider.h>
#include <bullet3\BulletCollision\CollisionShapes\btSphereShape.h>

using namespace ze;

SphereCollider::SphereCollider(float radius)
{
	m_upBtCollisionShape = std::make_unique<btSphereShape>(radius);
}
