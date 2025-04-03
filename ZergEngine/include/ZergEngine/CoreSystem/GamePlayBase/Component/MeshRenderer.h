#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	class Mesh;

	class MeshRenderer : public IComponent
	{
		friend class Graphics;
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::MESH_RENDERER;
		static inline bool IsCreatable() { return true; }
		MeshRenderer();
		virtual ~MeshRenderer() = default;
		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::MESH_RENDERER; }
	private:
		virtual IComponentManager* GetComponentManager() const override;
	public:
		std::shared_ptr<Mesh> m_mesh;
	};
}
