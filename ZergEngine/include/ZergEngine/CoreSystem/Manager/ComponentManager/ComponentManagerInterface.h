#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <ZergEngine\CoreSystem\SlimRWLock.h>

namespace ze
{
	class IComponent;

	class IComponentManager : public ISubsystem
	{
		friend class RuntimeImpl;
		friend class SceneManagerImpl;
		friend class RendererImpl;
		friend class ToPtrHelper;
		friend class GameObject;
		friend class IComponent;
	public:
		IComponentManager();
		virtual ~IComponentManager() = default;
	protected:
		void AddToDestroyQueue(IComponent* pComponent);

		ComponentHandleBase RegisterToHandleTable(IComponent* pComponent);
		virtual void AddPtrToActiveVector(IComponent* pComponent);
		virtual void RemoveDestroyedComponents();

		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	protected:
		uint64_t m_uniqueId;
		SlimRWLock m_lock;
		std::vector<IComponent*> m_destroyed;
		std::vector<IComponent*> m_activeComponents;
		std::vector<IComponent*> m_table;
	};
}
