#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class IComponent;

	class IComponentManager : public ISubsystem
	{
		friend class RuntimeImpl;
		friend class SceneManagerImpl;
		friend class RendererImpl;
		friend class HandleHelper;
		friend class GameObject;
		friend class IComponent;
	public:
		IComponentManager()
			: m_uniqueId(0)
			, m_destroyed()
			, m_enabledComponents()
			, m_disabledComponents()
			, m_table(128)
		{
		}
		virtual ~IComponentManager() = default;
	protected:
		void AddToDestroyQueue(IComponent* pComponent);
		void MoveToDisabledVectorFromEnabledVector(IComponent* pComponent);
		void MoveToEnabledVectorFromDisabledVector(IComponent* pComponent);

		void AddPtrToEnabledVector(IComponent* pComponent) { AddPtrToVector(m_enabledComponents, pComponent); }
		void AddPtrToDisabledVector(IComponent* pComponent) { AddPtrToVector(m_disabledComponents, pComponent); }
		static void AddPtrToVector(std::vector<IComponent*>& vector, IComponent* pComponent);
		void RemovePtrFromEnabledVector(IComponent* pComponent) { RemovePtrFromVector(m_enabledComponents, pComponent); }
		void RemovePtrFromDisabledVector(IComponent* pComponent) { RemovePtrFromVector(m_disabledComponents, pComponent); }
		static void RemovePtrFromVector(std::vector<IComponent*>& vector, IComponent* pComponent);
	protected:
		virtual ComponentHandleBase Register(IComponent* pComponent);
		virtual void RemoveDestroyedComponents();

		inline uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	protected:
		uint64_t m_uniqueId;
		std::vector<IComponent*> m_destroyed;
		std::vector<IComponent*> m_enabledComponents;
		std::vector<IComponent*> m_disabledComponents;
		std::vector<IComponent*> m_table;
	};
}
