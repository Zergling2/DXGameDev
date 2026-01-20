#include <ZergEngine\CoreSystem\Resource\BoxCollider.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <bullet3\BulletCollision\CollisionShapes\btBoxShape.h>

using namespace ze;

BoxCollider::BoxCollider(const XMFLOAT3& halfExtents)
{
	m_upBtCollisionShape = std::make_unique<btBoxShape>(DXToBt::ForwardVector(halfExtents));
}
