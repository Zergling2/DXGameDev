#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class Mesh;
	class Material;

	class MeshRenderer : public IComponent
	{
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::MESH_RENDERER;
		static constexpr bool IsCreatable() { return true; }

		MeshRenderer() noexcept;
		virtual ~MeshRenderer() = default;

		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::MESH_RENDERER; }

		bool GetCastShadows() const { return m_castShadows; }
		void SetCastShadows(bool b) { m_castShadows = b; }
		bool GetReceiveShadows() const { return m_receiveShadows; }
		void SetReceiveShadows(bool b) { m_receiveShadows = b; }

		const Mesh* GetMeshPtr() const { return m_spMesh.get(); }
		std::shared_ptr<Mesh> GetMesh() const { return m_spMesh; }
		void SetMesh(std::shared_ptr<Mesh> spMesh);

		size_t GetSubsetCount() const { return m_materials.size(); }
		// 범위검사 생략 (엔진 렌더러에서 사용, subsetIndex에 유효하지 않은 인덱스 전달하지 않도록 주의)
		Material* GetMaterialPtr(size_t subsetIndex) const { assert(subsetIndex < m_materials.size()); return m_materials[subsetIndex].get(); }

		// 범위검사 포함 (컨텐츠 로직에서 사용 권장)
		std::shared_ptr<Material> GetMaterial(size_t subsetIndex) const;
		bool SetMaterial(size_t subsetIndex, std::shared_ptr<Material> spMaterial);
	private:
		virtual IComponentManager* GetComponentManager() const override;
	private:
		std::shared_ptr<Mesh> m_spMesh;
		std::vector<std::shared_ptr<Material>> m_materials;
		bool m_castShadows;
		bool m_receiveShadows;
	};
}
