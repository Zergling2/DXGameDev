#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\SlimRWLock.h>

namespace ze
{
	class UIObjectManagerImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		friend class SceneManagerImpl;
		friend class RendererImpl;
		friend class IUIObject;
		friend class UIObjectHandle;
		friend class RectTransform;
		friend class IScene;
		friend class Button;
		ZE_DECLARE_SINGLETON(UIObjectManagerImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		UIObjectHandle RegisterToHandleTable(IUIObject* pUIObject);
		void AddToDestroyQueue(IUIObject* pUIObject);
		UIObjectHandle FindUIObject(PCWSTR name);

		void RemoveDestroyedUIObjects();

		void MoveToInactiveVectorFromActiveVector(IUIObject* pUIObject);
		void MoveToActiveVectorFromInactiveVector(IUIObject* pUIObject);

		void AddPtrToRootVector(IUIObject* pUIObject);
		void RemovePtrFromRootVector(IUIObject* pUIObject);

		void AddPtrToActiveVector(IUIObject* pUIObject) { AddPtrToVector(m_activeUIObjects, pUIObject); }
		void AddPtrToInactiveVector(IUIObject* pUIObject) { AddPtrToVector(m_inactiveUIObjects, pUIObject); }
		static void AddPtrToVector(std::vector<IUIObject*>& vector, IUIObject* pUIObject);
		void RemovePtrFromActiveVector(IUIObject* pUIObject) { RemovePtrFromVector(m_activeUIObjects, pUIObject); }
		void RemovePtrFromInactiveVector(IUIObject* pUIObject) { RemovePtrFromVector(m_inactiveUIObjects, pUIObject); }
		static void RemovePtrFromVector(std::vector<IUIObject*>& vector, IUIObject* pUIObject);

		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	private:
		uint64_t m_uniqueId;
		SlimRWLock m_lock;
		std::vector<IUIObject*> m_destroyed;
		std::vector<IUIObject*> m_rootUIObjects;	// 루트 UI 오브젝트들 포인터
		std::vector<IUIObject*> m_activeUIObjects;	// 검색 시 전체 테이블을 순회할 필요 제거
		std::vector<IUIObject*> m_inactiveUIObjects;	// 비활성화된 오브젝트들 (검색 대상에서 제외)
		std::vector<IUIObject*> m_table;
	};

	extern UIObjectManagerImpl UIObjectManager;
}
