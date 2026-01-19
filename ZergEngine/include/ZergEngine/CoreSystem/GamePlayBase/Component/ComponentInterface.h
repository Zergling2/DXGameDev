#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class IComponentManager;
	class GameObject;

	using ComponentFlagType = uint16_t;
	using cft = ComponentFlagType;

	enum class ComponentFlag : cft
	{
		None				= 0,
		Enabled				= 1 << 0,

		OnStartingQueue		= 1 << 13,
		StartCalled			= 1 << 14,
		OnDestroyQueue		= 1 << 15,
	};

	class IComponent
	{
		friend class Runtime;
		friend class IComponentManager;
		friend class MonoBehaviourManager;
		friend class SceneManager;
		friend class GameObject;
		friend class Transform;
		friend class Renderer;
		friend class DebugLinesEffect;
		friend class BasicEffectP;
		friend class BasicEffectPC;
		friend class BasicEffectPN;
		friend class BasicEffectPT;
		friend class BasicEffectPNT;
		friend class BasicEffectPNTT;
		friend class BasicEffectPNTTSkinned;
		friend class SkyboxEffect;
		friend class TerrainEffect;
		friend class BillboardEffect;
	public:
		IComponent(GameObject& owner, uint64_t id) noexcept;
		virtual ~IComponent() = default;

		virtual ComponentType GetType() const = 0;
		void Enable();
		void Disable();

		const GameObjectHandle GetGameObjectHandle() const;

		uint64_t GetId() const { return m_id; }
		bool IsEnabled() const { return static_cast<cft>(m_flag) & static_cast<cft>(ComponentFlag::Enabled); }
		void Destroy();
		void Destroy(float delay);

		bool IsOnTheDestroyQueue() const { return static_cast<cft>(m_flag) & static_cast<cft>(ComponentFlag::OnDestroyQueue); }
		bool IsOnTheStartingQueue() const { return static_cast<cft>(m_flag) & static_cast<cft>(ComponentFlag::OnStartingQueue); }
		bool IsStartCalled() const { return static_cast<cft>(m_flag) & static_cast<cft>(ComponentFlag::StartCalled); }
	protected:
		virtual void OnDeploySysJob();
		virtual void OnEnableSysJob();
		virtual void OnDisableSysJob();
	private:
		virtual IComponentManager* GetComponentManager() const = 0;

		const ComponentHandleBase ToHandle() const;
		void OnFlag(ComponentFlag flag) { m_flag = static_cast<ComponentFlag>(static_cast<cft>(m_flag) | static_cast<cft>(flag)); }
		void OffFlag(ComponentFlag flag) { m_flag = static_cast<ComponentFlag>(static_cast<cft>(m_flag) & ~static_cast<cft>(flag)); }
	protected:
		GameObject* m_pGameObject;
	private:
		uint64_t m_id;
		uint32_t m_tableIndex;
		uint32_t m_groupIndex;
		ComponentFlag m_flag;
		uint16_t m_reserved0;
		uint32_t m_reserved1;
	};
}
