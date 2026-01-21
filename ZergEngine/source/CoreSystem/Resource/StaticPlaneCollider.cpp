#include <ZergEngine\CoreSystem\Resource\StaticPlaneCollider.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <bullet3\BulletCollision\CollisionShapes\btStaticPlaneShape.h>

using namespace ze;

StaticPlaneCollider::StaticPlaneCollider(const XMFLOAT3& planeNormal, float planeConstant)
{
	m_upBtCollisionShape = std::make_unique<btStaticPlaneShape>(DXToBt::ConvertVector(planeNormal), planeConstant);
}
