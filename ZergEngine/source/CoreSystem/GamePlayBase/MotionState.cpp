#include <ZergEngine\CoreSystem\GamePlayBase\MotionState.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>

using namespace ze;

void MotionState::getWorldTransform(btTransform& worldTrans) const
{
	XMVECTOR scaleW;
	XMVECTOR rotationW;
	XMVECTOR positionW;

	m_transform.GetWorldTransform(&scaleW, &rotationW, &positionW);

	XMFLOAT4A dxRotW;
	XMFLOAT3A dxPosW;
	XMStoreFloat4A(&dxRotW, rotationW);
	XMStoreFloat3A(&dxPosW, positionW);

	worldTrans.setRotation(DXToBt::ConvertQuaternion(dxRotW));
	worldTrans.setOrigin(DXToBt::ConvertVector(dxPosW));
}

void MotionState::setWorldTransform(const btTransform& worldTrans)
{
	XMFLOAT3 dxPos = BtToDX::ConvertVector(worldTrans.getOrigin());
	XMFLOAT4 dxRot = BtToDX::ConvertQuaternion(worldTrans.getRotation());

	XMVECTOR worldPos = XMLoadFloat3(&dxPos);
	XMVECTOR worldRot = XMLoadFloat4(&dxRot);

	if (m_transform.GetParent())
	{
		// 월드 위치, 월드 회전을 로컬 위치, 로컬 회전으로 변환해야 함.
		// 부모의 월드 위치를 Pp, 부모의 월드 회전을 Qp라고 하고
		// 자식의 월드 위치를 Pw, 월드 회전을 Qw, 로컬 위치를 Pl, 로컬 회전을 Ql라고 하면
		// World Pos = (Local Pos * Parent World Rot) + Parent World Pos
		// 따라서 Local Pos = (World Pos - Parent World Pos) * Inv(Parent World Rot)
		// 
		// World Rot = Local Rot * Parent World Rot
		// 따라서 Local Rot = World Rot * Inv(Parent World Rot)

		XMVECTOR parentWorldScale;
		XMVECTOR parentWorldRot;
		XMVECTOR parentWorldPos;
		m_transform.GetParent()->GetWorldTransform(&parentWorldScale, &parentWorldRot, &parentWorldPos);

		XMVECTOR invParentWorldRot = XMQuaternionInverse(parentWorldRot);
		
		XMVECTOR localPos = XMVector3Rotate(XMVectorSubtract(worldPos, parentWorldPos), invParentWorldRot);

		// v' = qvq-1
		// localRot = invParentWorldRot(worldRot(v))
		XMVECTOR localRot = XMQuaternionMultiply(invParentWorldRot, worldRot);	// worldRot먼저, invParentWorldRot 나중에

		m_transform.SetPosition(localPos);
		m_transform.SetRotationQuaternion(localRot);
	}
	else
	{
		m_transform.SetPosition(worldPos);
		m_transform.SetRotationQuaternion(worldRot);
	}
}
