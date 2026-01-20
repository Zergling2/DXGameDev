#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <bullet3\LinearMath\btMotionState.h>

namespace ze
{
	class Transform;

	class MotionState : public btMotionState
	{
	public:
		MotionState(Transform& transform, const XMFLOAT3& localPos, const XMFLOAT4& localRot);
		virtual ~MotionState() = default;

		virtual void getWorldTransform(btTransform& worldTrans) const override;
		virtual void setWorldTransform(const btTransform& worldTrans) override;

		void SetLocalPos(const XMFLOAT3& localPos);
		void SetLocalRot(const XMFLOAT4& localRot);
	private:
		Transform& m_transform;
		XMFLOAT3A m_localPos;	// Rigidbody collider local transform
		XMFLOAT4A m_localRot;	// Rigidbody collider local transform
	};
}
