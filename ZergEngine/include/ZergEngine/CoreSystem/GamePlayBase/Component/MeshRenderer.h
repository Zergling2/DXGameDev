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
	private:
		virtual IComponentManager* GetComponentManager() const override;
	public:
		std::shared_ptr<Mesh> m_mesh;
	private:
		bool m_castShadows;
		bool m_receiveShadows;
	};
}
