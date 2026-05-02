#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\Resource\ColliderInterface.h>

namespace ze
{
	class CapsuleCollider : public ICollider
	{
	public:
		/**
		* @param radius 캡슐의 반지름입니다.
		* @param height 캡슐의 두 cap 사이의 거리를 나타냅니다. 따라서 캡슐의 최종 높이는 2 x radius + height입니다.
		*/
		CapsuleCollider(float radius, float height);
		virtual ~CapsuleCollider() = default;

		virtual ColliderType GetType() const override { return ColliderType::Capsule; }
		float GetRadius() const { return m_radius; }
		float GetHeight() const { return m_height; }
		float GetTotalHeight() const { return 2 * m_radius + m_height; }
	private:
		float m_radius;
		float m_height;
	};
}
