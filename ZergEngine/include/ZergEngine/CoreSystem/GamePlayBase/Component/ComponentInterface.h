#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class IComponentManager;

	class IComponent
	{
		friend class RuntimeImpl;
		friend class IComponentManager;
		friend class SceneManagerImpl;
		friend class GameObject;
		friend class Camera;
		friend class RendererImpl;
		friend class BasicEffectP;
		friend class BasicEffectPC;
		friend class BasicEffectPN;
		friend class BasicEffectPT;
		friend class BasicEffectPNT;
		friend class SkyboxEffect;
	public:
		IComponent(uint64_t id) noexcept
			: m_pGameObject(nullptr)
			, m_id(id)
			, m_tableIndex(std::numeric_limits<uint32_t>::max())
			, m_activeIndex(std::numeric_limits<uint32_t>::max())
			, m_onDestroyQueue(false)
			, m_enabled(true)
		{
		}
		virtual ~IComponent() = default;

		virtual COMPONENT_TYPE GetType() const = 0;
		const GameObjectHandle GetGameObjectHandle() const;
		uint64_t GetId() const { return m_id; }
		bool IsEnabled() const { return m_enabled; }
		void Enable();
		void Disable();
	private:
		ComponentHandleBase ToHandleBase() const;
		virtual IComponentManager* GetComponentManager() const = 0;
		virtual void SystemJobOnEnabled();
		virtual void SystemJobOnDisabled();

		void SetOnDestroyQueueFlag() { m_onDestroyQueue = true; }
		bool IsOnTheDestroyQueue() const { return m_onDestroyQueue; }
	private:
		GameObject* m_pGameObject;
		uint64_t m_id;
		uint32_t m_tableIndex;
		uint32_t m_activeIndex;
		bool m_onDestroyQueue;
		bool m_enabled;
	};
}
