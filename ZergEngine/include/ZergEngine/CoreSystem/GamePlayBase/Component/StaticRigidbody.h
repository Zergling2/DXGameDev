#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\RigidbodyInterface.h>

namespace ze
{
	// Static Rigidbody의 월드 트랜스폼은 이 컴포넌트가 AddComponent 함수로 추가될 때의 GameObject 트랜스폼을 기준으로 설정되며 이후에는 변경할 수 없습니다.
	class StaticRigidbody : public IRigidbody
	{
	public:
		static constexpr ComponentType TYPE = ComponentType::StaticRigidbody;
		static bool IsCreatable() { return true; }

		StaticRigidbody(GameObject& owner, std::shared_ptr<ICollider> collider, const XMFLOAT3& localPos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT4& localRot = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		virtual ~StaticRigidbody() = default;

		virtual ComponentType GetType() const override { return ComponentType::StaticRigidbody; }
	};
}
