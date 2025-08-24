#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class IComponentManager;
	class GameObject;

	using ComponentFlagType = uint16_t;
	using cft = ComponentFlagType;

	enum class COMPONENT_FLAG : cft
	{
		NONE				= 0,
		ENABLED				= 1 << 0,

		ON_STARTING_QUEUE	= 1 << 13,
		START_CALLED		= 1 << 14,
		ON_DESTROY_QUEUE	= 1 << 15,
	};

	class IComponent
	{
		friend class Runtime;
		friend class IComponentManager;
		friend class MonoBehaviourManager;
		friend class SceneManager;
		friend class GameObject;
		friend class Camera;
		friend class Transform;
		friend class Renderer;
		friend class BasicEffectP;
		friend class BasicEffectPC;
		friend class BasicEffectPN;
		friend class BasicEffectPT;
		friend class BasicEffectPNT;
		friend class SkyboxEffect;
		friend class TerrainEffect;
	public:
		IComponent(uint64_t id) noexcept;
		virtual ~IComponent() = default;

		virtual COMPONENT_TYPE GetType() const = 0;
		virtual void Enable();
		virtual void Disable();

		const GameObjectHandle GetGameObjectHandle() const;

		uint64_t GetId() const { return m_id; }
		bool IsEnabled() const { return static_cast<cft>(m_flag) & static_cast<cft>(COMPONENT_FLAG::ENABLED); }
		void Destroy();
		void Destroy(float delay);

		bool IsOnTheDestroyQueue() const { return static_cast<cft>(m_flag) & static_cast<cft>(COMPONENT_FLAG::ON_DESTROY_QUEUE); }
		bool IsOnTheStartingQueue() const { return static_cast<cft>(m_flag) & static_cast<cft>(COMPONENT_FLAG::ON_STARTING_QUEUE); }
		bool IsStartCalled() const { return static_cast<cft>(m_flag) & static_cast<cft>(COMPONENT_FLAG::START_CALLED); }
	private:
		virtual IComponentManager* GetComponentManager() const = 0;

		const ComponentHandleBase ToHandleBase() const;
		void OnFlag(COMPONENT_FLAG flag) { m_flag = static_cast<COMPONENT_FLAG>(static_cast<cft>(m_flag) | static_cast<cft>(flag)); }
		void OffFlag(COMPONENT_FLAG flag) { m_flag = static_cast<COMPONENT_FLAG>(static_cast<cft>(m_flag) & ~static_cast<cft>(flag)); }
	private:
		GameObject* m_pGameObject;
		uint64_t m_id;
		uint32_t m_tableIndex;
		uint32_t m_groupIndex;
		COMPONENT_FLAG m_flag;
		uint16_t m_reserved0;
		uint32_t m_reserved1;
	};
}
