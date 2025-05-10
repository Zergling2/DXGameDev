#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class IComponentManager;

	using ComponentFlagType = uint16_t;

	enum COMPONENT_FLAG : ComponentFlagType
	{
		CF_NONE					= 0,
		CF_ENABLED				= 1 << 0,

		CF_ON_STARTING_QUEUE	= 1 << 13,
		CF_START_CALLED			= 1 << 14,
		CF_ON_DESTROY_QUEUE		= 1 << 15,
	};

	class IComponent
	{
		friend class RuntimeImpl;
		friend class IComponentManager;
		friend class SceneManagerImpl;
		friend class ScriptManagerImpl;
		friend class GameObject;
		friend class Camera;
		friend class Transform;
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
			, m_flag(CF_ENABLED)	// 기본 상태는 enable 상태
		{
		}
		virtual ~IComponent() = default;

		virtual COMPONENT_TYPE GetType() const = 0;
		const GameObjectHandle GetGameObjectHandle() const;
		uint64_t GetId() const { return m_id; }
		bool IsEnabled() const { return m_flag & CF_ENABLED; }
		virtual bool Enable();
		virtual bool Disable();

		ComponentFlagType GetFlag() const { return m_flag; }
	private:
		virtual IComponentManager* GetComponentManager() const = 0;

		ComponentHandleBase ToHandleBase() const;
		void OnFlag(ComponentFlagType flag) { m_flag |= flag; }
		void OffFlag(ComponentFlagType flag) { m_flag &= ~flag; }
		bool IsOnTheDestroyQueue() const { return m_flag & CF_ON_DESTROY_QUEUE; }
	private:
		GameObject* m_pGameObject;

		uint64_t m_id;

		uint32_t m_tableIndex;
		uint32_t m_activeIndex;

		ComponentFlagType m_flag;
		uint16_t m_reserved0;
		uint32_t m_reserved1;
	};
}
