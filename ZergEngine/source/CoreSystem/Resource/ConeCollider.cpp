#include <ZergEngine\CoreSystem\Resource\ConeCollider.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <bullet3\BulletCollision\CollisionShapes\btConeShape.h>

using namespace ze;

ConeCollider::ConeCollider(float radius, float height)
{
	m_upBtCollisionShape = std::make_unique<btConeShape>(radius, height);
}
