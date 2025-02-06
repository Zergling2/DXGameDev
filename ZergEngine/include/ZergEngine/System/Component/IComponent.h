#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class GameObject;

	enum class COMPONENT_TYPE : uint16_t
	{
		CAMERA,
		MESH_RENDERER,
		DIRECTIONAL_LIGHT,
		POINT_LIGHT,
		SPOT_LIGHT,
		SCRIPT,
		RIGID_BODY,
		BOX_COLLIDER,
		SPHERE_COLLIDER,
		MESH_COLLIDER,
		UNKNOWN
	};

	class IComponent
	{
		friend class GameObject;
		friend class SceneManager;
		friend class IScene;
		friend class Graphics;
		friend class ComponentSystem;
		friend void Destroy(std::shared_ptr<IComponent>& component);
	public:
		IComponent()
			: m_enabled(true)
			, m_pGameObject(nullptr)
		{
		}
		virtual ~IComponent() = 0;
		virtual COMPONENT_TYPE GetType() const = 0;
		inline bool IsDestroyed() const { return m_pGameObject == nullptr; }
		inline bool IsEnabled() const { return m_enabled; }
		bool Enable();
		bool Disable();

		inline GameObject* GetGameObject() const { return m_pGameObject; }
	private:
		virtual void SystemJobOnEnabled() = 0;
		virtual void SystemJobOnDisabled() = 0;
	private:
		bool m_enabled;
		GameObject* m_pGameObject;		// component owner
	};
}
