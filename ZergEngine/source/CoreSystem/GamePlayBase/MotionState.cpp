#include <ZergEngine\CoreSystem\GamePlayBase\MotionState.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>
#include <ZergEngine\CoreSystem\BulletDXMath.h>

using namespace ze;

MotionState::MotionState(Transform& transform, const XMFLOAT3& localPos, const XMFLOAT4& localRot)
	: m_transform(transform)
{
	this->SetLocalPos(localPos);
	this->SetLocalRot(localRot);
}

void MotionState::getWorldTransform(btTransform& worldTrans) const
{
	XMVECTOR vScaleW;
	XMVECTOR vWorldRot;
	XMVECTOR vWorldPos;

	m_transform.GetWorldTransform(&vScaleW, &vWorldRot, &vWorldPos);

	// Compute Rigidbody Final World Transform (오프셋을 감안한 Rigidbody의 월드 트랜스폼 계산)
	XMVECTOR vRbLocalRot = XMLoadFloat4A(&m_localRot);
	XMVECTOR vRbLocalPos = XMLoadFloat3A(&m_localPos);

	XMVECTOR vRbWorldRot = XMQuaternionMultiply(vRbLocalRot, vWorldRot);
	XMVECTOR vRbWorldPos = XMVectorAdd(XMVector3Rotate(vRbLocalPos, vWorldRot), vWorldPos);
	
	XMFLOAT4 rbWorldRot;
	XMFLOAT3 rbWorldPos;
	XMStoreFloat4(&rbWorldRot, vRbWorldRot);
	XMStoreFloat3(&rbWorldPos, vRbWorldPos);

	worldTrans.setRotation(DXToBt::ConvertQuaternion(rbWorldRot));
	worldTrans.setOrigin(DXToBt::ConvertVector(rbWorldPos));
}

void MotionState::setWorldTransform(const btTransform& worldTrans)
{
	const XMFLOAT4 rbWorldRot = BtToDX::ConvertQuaternion(worldTrans.getRotation());
	const XMFLOAT3 rbWorldPos = BtToDX::ConvertVector(worldTrans.getOrigin());
	XMVECTOR vRbWorldRot = XMLoadFloat4(&rbWorldRot);
	XMVECTOR vRbWorldPos = XMLoadFloat3(&rbWorldPos);


	// getWorldTransform에서 Rigidbody transform은 이렇게 계산되었음. (vWorldRot, vWorldPos는 GameObject의 Transform)
	// vRbWorldRot = QuaternionMultiply(m_localRot, vWorldRot)
	// vRbWorldPos = XMVectorAdd(XMVector3Rotate(m_localPos, vWorldRot), vWorldPos);

	// 수학 수식으로 나타내면
	// vRbWorldRot = vWorldRot * m_localRot
	// vRbWorldPos = vWorldPos + m_localPos * vWorldRot

	// 따라서 역으로 
	// vRbWorldRot * inv(m_localRot) = vWorldRot
	// vRbWorldPos - m_localPos * vWorldRot = vWorldPos


	XMVECTOR vRbInvLocalRot = XMQuaternionConjugate(XMLoadFloat4A(&m_localRot));

	XMVECTOR vWorldRot = XMQuaternionMultiply(vRbInvLocalRot, vRbWorldRot);
	XMVECTOR vWorldPos = XMVectorSubtract(vRbWorldPos, XMVector3Rotate(XMLoadFloat3(&m_localPos), vWorldRot));

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

		XMVECTOR vInvParentWorldRot = XMQuaternionConjugate(parentWorldRot);
		
		XMVECTOR vLocalPos = XMVector3Rotate(XMVectorSubtract(vWorldPos, parentWorldPos), vInvParentWorldRot);

		// v' = qvq-1
		// vLocalRot = vInvParentWorldRot(vWorldRot(v))
		XMVECTOR vLocalRot = XMQuaternionMultiply(vWorldRot, vInvParentWorldRot);	// vWorldRot먼저, vInvParentWorldRot 나중에

		m_transform.SetPosition(vLocalPos);
		m_transform.SetRotationQuaternion(vLocalRot);
	}
	else
	{
		// local pos == world pos
		// local rot == world rot
		m_transform.SetPosition(vWorldPos);
		m_transform.SetRotationQuaternion(vWorldRot);
	}
}

void MotionState::SetLocalPos(const XMFLOAT3& localPos)
{
	m_localPos.x = localPos.x;
	m_localPos.y = localPos.y;
	m_localPos.z = localPos.z;
}

void MotionState::SetLocalRot(const XMFLOAT4& localRot)
{
	m_localRot.x = localRot.x;
	m_localRot.y = localRot.y;
	m_localRot.z = localRot.z;
	m_localRot.w = localRot.w;
}
