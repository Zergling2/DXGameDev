#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class IComponentManager;

	class IComponent
	{
		friend class Runtime;
		friend class IComponentManager;
		friend class SceneManager;
	public:
		IComponent();
		virtual ~IComponent() = default;
		virtual COMPONENT_TYPE GetType() const = 0;
		const GameObjectHandle& GetGameObjectHandle() const { return m_hGameObject; }
		inline uint64_t GetId() const { return m_id; }
		inline bool IsEnabled() const { return m_enabled; }
		bool Enable();
		bool Disable();
	protected:
		inline void SetId(uint64_t id) { m_id = id; }
	private:
		virtual IComponentManager* GetComponentManager() const = 0;
		virtual void SystemJobOnEnabled();
		virtual void SystemJobOnDisabled();
	private:
		GameObjectHandle m_hGameObject;
		uint64_t m_id;
		uint32_t m_index;
		uint32_t m_activeIndex;
		bool m_enabled;
	};
}
