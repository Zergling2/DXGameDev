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
		friend class ComponentHandle;
	public:
		IComponentManager();
		virtual ~IComponentManager() = default;
	protected:
		virtual ComponentHandle Register(IComponent* pComponent);
		virtual void Unregister(IComponent* pComponent);

		inline uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	protected:
		uint64_t m_uniqueId;
		std::vector<IComponent*> m_activeComponents;	// 업데이트 시 전체 테이블을 순회할 필요를 없앤다.
		IComponent* m_ptrTable[64 * 128];
	};
}
