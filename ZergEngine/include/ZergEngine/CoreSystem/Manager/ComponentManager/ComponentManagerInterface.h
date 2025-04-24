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
		IComponentManager();
		virtual ~IComponentManager() = default;
	private:
		void AddToDestroyQueue(IComponent* pComponent);
	protected:
		virtual ComponentHandleBase Register(IComponent* pComponent);
		virtual void RemoveDestroyedComponents();

		inline uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	protected:
		uint64_t m_uniqueId;
		std::vector<IComponent*> m_destroyed;
		std::vector<IComponent*> m_activeComponents;	// ������Ʈ �� ��ü ���̺��� ��ȸ�� �ʿ並 ���ش�.
		std::vector<IComponent*> m_table;
	};
}
