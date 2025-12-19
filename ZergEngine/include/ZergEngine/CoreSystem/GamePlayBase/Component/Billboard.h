#pragma once

#include <ZergEngine\CoreSystem\Platform.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>
#include <memory>

namespace ze
{
	class Material;

	enum class BillboardType
	{
		Spherical,		// 빌보드가 항상 카메라를 향합니다. 빌보드의 회전이 극좌표계를 따릅니다.
		CylindricalY,	// Spherical 방식에서 Y축 회전만을 허용합니다.
		ScreenAligned	// 빌보드가 항상 스크린 평면을 향하여 정렬됩니다.
	};

	enum class BillboardProjection
	{
		Perspective,
		Orthographic
	};

	// 빌보드 사각형의 크기는 Transform의 Scale값에 영향을 받습니다.
	// Scale의 X값과 Y값은 각각 너비와 높이에 곱해지며, Z값은 무시됩니다.
	class Billboard : public IComponent
	{
		friend class Renderer;
	public:
		static constexpr ComponentType TYPE = ComponentType::Billboard;
		static constexpr bool IsCreatable() { return true; }

		Billboard() noexcept;
		virtual ~Billboard() = default;

		virtual ComponentType GetType() const override { return ComponentType::Billboard; }

		BillboardType GetBillboardType() const { return m_billboardType; }
		void SetBillboardType(BillboardType type) { m_billboardType = type; }
		// BillboardProjection GetBillboardProjection() const { return m_billboardProj; }
		// void SetBillboardProjection(BillboardProjection proj) const { m_billboardProj = proj; }
		float GetBillboardQuadWidth() const { return m_quadWidth; }
		void SetBillboardQuadWidth(float width) { m_quadWidth = width; }
		float GetBillboardQuadHeight() const { return m_quadHeight; }
		void SetBillboardQuadHeight(float height) { m_quadHeight = height; }
		const Material* GetMaterialPtr() const { return m_spMaterial.get(); }
		void SetMaterial(std::shared_ptr<Material> spMaterial) { m_spMaterial = std::move(spMaterial); }
	private:
		virtual IComponentManager* GetComponentManager() const override;

		void XM_CALLCONV CacheWorldMatrix(FXMMATRIX worldMatrix) { XMStoreFloat4x4A(&m_worldMatrixCache, worldMatrix); }
		FXMMATRIX GetWorldMatrixCache() const { return XMLoadFloat4x4A(&m_worldMatrixCache); }
	private:
		BillboardType m_billboardType;
		// BillboardProjection m_billboardProj;
		float m_quadWidth;
		float m_quadHeight;
		std::shared_ptr<Material> m_spMaterial;

		XMFLOAT4X4A m_worldMatrixCache;	// Cache!
	};
}
