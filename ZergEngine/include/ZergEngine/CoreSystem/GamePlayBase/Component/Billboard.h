#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <memory>

namespace ze
{
	class Material;

	enum class BillboardType
	{
		Spherical,		// 빌보드 사각형이 항상 카메라를 향합니다.
		CylindricalY,	// Spherical 방식에서 Y축 회전만을 허용합니다.
		CylindricalX,	// Spherical 방식에서 X축 회전만을 허용합니다.
		CylindricalZ,	// Spherical 방식에서 Z축 회전만을 허용합니다.
		ScreenAligned	// 빌보드 사각형이 항상 스크린 평면을 향합니다.
	};

	enum class BillboardProjection
	{
		Perspective,
		Orthographic
	};

	// 빌보드 사각형의 크기는 빌보드 컴포넌트를 소유한 GameObject Transform의 Scale값에 의해 결정됩니다.
	// Scale의 X값은 빌보드 사각형의 너비를, Y값은 빌보드 사각형의 높이를 결정합니다. Z값은 무시됩니다.
	class Billboard : public IComponent
	{
	public:
		static constexpr ComponentType TYPE = ComponentType::Billboard;
		static constexpr bool IsCreatable() { return true; }

		Billboard() noexcept;
		virtual ~Billboard() = default;

		virtual ComponentType GetType() const override { return ComponentType::Billboard; }

	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		BillboardType m_billboardType;
		BillboardProjection m_billboardProj;
		std::shared_ptr<Material> m_spMaterial;
	};
}
