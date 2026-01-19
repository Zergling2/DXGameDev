#include <ZergEngine\CoreSystem\Resource\BoxCollider.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <bullet3\BulletCollision\CollisionShapes\btCompoundShape.h>

using namespace ze;

BoxCollider::BoxCollider(const XMFLOAT3& halfExtents, const XMFLOAT3& localPos, const XMFLOAT4& localRot)
{
	// Child Shape 제거는 btCompoundShape의 첫 번째 생성자에 false를 사용해야만 가능하다고 함! (btCompoundShape 헤더파일 설명)
	m_upBtCollisionShape = std::make_unique<btCompoundShape>(true, 1);
	
	btTransform localTransform;
	// localTransform.setIdentity();	// 바로 아래에서 setRotation 호출하므로 불필요
	localTransform.setRotation(DXToBt::ConvertQuaternion(localRot));
	localTransform.setOrigin(DXToBt::ConvertVector(localPos));

	btCollisionShape* pBtCollisionShape = new btBoxShape(DXToBt::ForwardVector(halfExtents));
	m_upBtCollisionShape->addChildShape(localTransform, pBtCollisionShape);
}

BoxCollider::~BoxCollider()
{
	for (int i = 0; i < m_upBtCollisionShape->getNumChildShapes(); ++i)
		delete m_upBtCollisionShape->getChildShape(i);
}

btCollisionShape* BoxCollider::GetCollisionShape() const
{
	return m_upBtCollisionShape.get();
}
