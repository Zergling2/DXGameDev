#pragma once

#include <ZergEngine\System\Component\IComponent.h>

namespace zergengine
{
	class Mesh;

	class MeshRenderer : public IComponent
	{
		friend class Graphics;
	public:
		static constexpr COMPONENT_TYPE TYPE = COMPONENT_TYPE::MESH_RENDERER;
		static inline bool IsCreatable() { return true; }
		MeshRenderer()
			: m_mesh(nullptr)
		{
		}
		virtual ~MeshRenderer()
		{
		}
		virtual COMPONENT_TYPE GetType() const override { return COMPONENT_TYPE::MESH_RENDERER; }
	private:
		virtual void SystemJobOnEnabled() override;
		virtual void SystemJobOnDisabled() override;
	public:
		std::shared_ptr<Mesh> m_mesh;
	};
}
