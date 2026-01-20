#include <ZergEngine\CoreSystem\Resource\CylinderCollider.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <bullet3\BulletCollision\CollisionShapes\btCylinderShape.h>

using namespace ze;

CylinderCollider::CylinderCollider(float radius, float height)
{
	m_upBtCollisionShape = std::make_unique<btCylinderShape>(btVector3(radius, height, radius));
}
