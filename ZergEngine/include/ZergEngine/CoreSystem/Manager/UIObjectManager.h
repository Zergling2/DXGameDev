#pragma once

#include <ZergEngine\CoreSystem\SlimRWLock.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>

namespace ze
{
	class IUIObject;
	class RectTransform;

	class UIObjectManager
	{
		friend class Runtime;
		friend class Renderer;
		friend class IScene;
		friend class IUIObject;
		friend class UIObjectHandle;
		friend class RectTransform;
		friend class InputField;
	public:
		static UIObjectManager* GetInstance() { return s_pInstance; }

		IUIObject* GetFocusedUI() const { return m_pFocusedUIObject; }
		void SetFocusedUI(IUIObject* pUIObject) { m_pFocusedUIObject = pUIObject; }
	private:
		UIObjectManager();
		~UIObjectManager();

		static void CreateInstance();
		static void DestroyInstance();

		void Init();
		void UnInit();

		void Deploy(IUIObject* pUIObject);

		template<typename T>
		UIObjectHandle CreateObject(PCWSTR name);

		template<typename T>
		UIObjectHandle CreatePendingObject(T** ppNewUIObject, PCWSTR name);

		void RequestDestroy(IUIObject* pUIObject);

		IUIObject* ToPtr(uint32_t tableIndex, uint64_t id) const;

		UIObjectHandle RegisterToHandleTable(IUIObject* pUIObject);
		void AddToDestroyQueue(IUIObject* pUIObject);
		UIObjectHandle FindUIObject(PCWSTR name);

		void MoveToActiveGroup(IUIObject* pUIObject);
		void MoveToInactiveGroup(IUIObject* pUIObject);

		void AddToRootArray(IUIObject* pUIObject);
		void DeployToActiveGroup(IUIObject* pUIObject);
		void DeployToInactiveGroup(IUIObject* pUIObject);
		void AddToActiveGroup(IUIObject* pUIObject);
		void AddToInactiveGroup(IUIObject* pUIObject);

		void RemoveFromRootArray(IUIObject* pUIObject);	// Root group에서 포인터 제거
		void RemoveFromGroup(IUIObject* pUIObject);	// Active/Inactive group에서 포인터 제거
		void RemoveDestroyedUIObjects();

		void SetActive(IUIObject* pUIObject, bool active);

		// pTransform은 항상 nullptr이 아닌 입력
		// pNewTransform은 nullptr이 입력으로 들어올 수 있음.
		bool SetParent(RectTransform* pTransform, RectTransform* pNewParentTransform);

		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }

		IUIObject* XM_CALLCONV SearchForHitUI(const XMFLOAT2& mousePos);
		static IUIObject* XM_CALLCONV PostOrderHitTest(const XMFLOAT2& mousePos, IUIObject* pUIObject);

		// Windows Vista부터 Surrogate pair 전달 가능
		void OnChar(WPARAM wParam, LPARAM lParam);
		void OnLButtonDown(POINT pt);
		void OnLButtonUp(POINT pt);
		void OnMButtonDown(POINT pt);
		void OnMButtonUp(POINT pt);
		void OnRButtonDown(POINT pt);
		void OnRButtonUp(POINT pt);

		void DetachUIFromManager(IUIObject* pUIObject);
	private:
		static UIObjectManager* s_pInstance;

		uint64_t m_uniqueId;
		SlimRWLock m_lock;
		std::vector<IUIObject*> m_destroyed;
		std::vector<IUIObject*> m_roots;			// 루트 UI 오브젝트들 포인터 (렌더링 시 루트부터 렌더링하기 위해서 필요)
		std::vector<IUIObject*> m_activeGroup;		// 검색 시 전체 테이블을 순회할 필요 제거
		std::vector<IUIObject*> m_inactiveGroup;	// 비활성화된 오브젝트들 (검색 대상에서 제외)
		std::vector<uint32_t> m_emptyHandleTableIndex;
		std::vector<IUIObject*> m_handleTable;
		IUIObject* m_pLButtonPressedObject;
		IUIObject* m_pMButtonPressedObject;
		IUIObject* m_pRButtonPressedObject;
		IUIObject* m_pFocusedUIObject;
	};

	template<typename T>
	UIObjectHandle UIObjectManager::CreateObject(PCWSTR name)
	{
		UIOBJECT_FLAG flag = UIOBJECT_FLAG::ACTIVE;

		T* pNewUIObject = new T(this->AssignUniqueId(), flag, name);
		UIObjectHandle hNewUIObject = this->RegisterToHandleTable(pNewUIObject);

		this->AddToRootArray(pNewUIObject);
		this->AddToActiveGroup(pNewUIObject);

		return hNewUIObject;
	}

	template<typename T>
	UIObjectHandle UIObjectManager::CreatePendingObject(T** ppNewUIObject, PCWSTR name)
	{
		// REAL_ROOT 플래그 활성화 및 Root Vector에 추가는 SceneManager에서 씬 변경 시 수행한다.
		UIOBJECT_FLAG flag =
			static_cast<UIOBJECT_FLAG>(static_cast<uint16_t>(UIOBJECT_FLAG::PENDING) | static_cast<uint16_t>(UIOBJECT_FLAG::ACTIVE));

		// PENDING GAME OBJECT
		T* pNewUIObject = new T(this->AssignUniqueId(), flag, name);
		*ppNewUIObject = pNewUIObject;
		return this->RegisterToHandleTable(pNewUIObject);
	}
}
