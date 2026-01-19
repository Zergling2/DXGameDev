#pragma once

#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <ZergEngine\CoreSystem\SlimRWLock.h>
#include <vector>

namespace ze
{
	class IComponent;

	class IComponentManager
	{
		friend class Runtime;
		friend class IComponent;
		friend class GameObject;
		friend class ToPtrHelper;
	protected:
		IComponentManager();
		virtual ~IComponentManager() = default;

		virtual void Init();
		virtual void Shutdown();

		void RequestDestroy(IComponent* pComponent);
		IComponent* ToPtr(uint32_t tableIndex, uint64_t id) const;

		ComponentHandleBase RegisterToHandleTable(IComponent* pComponent);

		virtual void AddToDirectAccessGroup(IComponent* pComponent);
		virtual void RemoveDestroyedComponents();

		void AddToDestroyQueue(IComponent* pComponent);
		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	protected:
		uint64_t m_uniqueId;
		SlimRWLock m_lock;
		std::vector<IComponent*> m_destroyed;
		std::vector<IComponent*> m_directAccessGroup;
		std::vector<uint32_t> m_emptyHandleTableIndex;
		std::vector<IComponent*> m_handleTable;
	};
}
