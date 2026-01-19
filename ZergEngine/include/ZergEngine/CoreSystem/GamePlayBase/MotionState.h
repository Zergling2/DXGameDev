#pragma once

#include <bullet3\LinearMath\btMotionState.h>

namespace ze
{
	class Transform;

	class MotionState : public btMotionState
	{
	public:
		MotionState(Transform& transform)
			: m_transform(transform)
		{
		}
		virtual ~MotionState() = default;

		virtual void getWorldTransform(btTransform& worldTrans) const override;
		virtual void setWorldTransform(const btTransform& worldTrans) override;
	private:
		Transform& m_transform;
	};
}
