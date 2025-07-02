#pragma once

#include <ZergEngine\CoreSystem\SlimRWLock.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>

#define THREADSAFE

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
	public:
		static UIObjectManager* GetInstance() { return s_pInstance; }
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

		UIObjectHandle THREADSAFE RegisterToHandleTable(IUIObject* pUIObject);
		void AddToDestroyQueue(IUIObject* pUIObject);
		UIObjectHandle FindUIObject(PCWSTR name);

		void MoveToActiveGroup(IUIObject* pUIObject);
		void MoveToInactiveGroup(IUIObject* pUIObject);

		void AddToRootArray(IUIObject* pUIObject);
		void DeployToActiveGroup(IUIObject* pUIObject);
		void DeployToInactiveGroup(IUIObject* pUIObject);
		void AddToActiveGroup(IUIObject* pUIObject);
		void AddToInactiveGroup(IUIObject* pUIObject);

		void RemoveFromRootArray(IUIObject* pUIObject);	// Root group���� ������ ����
		void RemoveFromGroup(IUIObject* pUIObject);	// Active/Inactive group���� ������ ����
		void RemoveDestroyedUIObjects();

		void SetActive(IUIObject* pUIObject, bool active);

		// pTransform�� �׻� nullptr�� �ƴ� �Է�
		// pNewTransform�� nullptr�� �Է����� ���� �� ����.
		bool SetParent(RectTransform* pTransform, RectTransform* pNewParentTransform);

		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }

		// Windows Vista���� Surrogate pair ���� ����
		void OnChar(WPARAM wParam, LPARAM lParam) {}
		void OnLButtonDown(POINT pt);
		void OnLButtonUp(POINT pt);
		void OnRButtonDown(POINT pt);
		void OnRButtonUp(POINT pt);
		void OnMButtonDown(POINT pt);
		void OnMButtonUp(POINT pt);
	private:
		static UIObjectManager* s_pInstance;

		uint64_t m_uniqueId;
		SlimRWLock m_lock;
		std::vector<IUIObject*> m_destroyed;
		std::vector<IUIObject*> m_roots;			// ��Ʈ UI ������Ʈ�� ������ (������ �� ��Ʈ���� �������ϱ� ���ؼ� �ʿ�)
		std::vector<IUIObject*> m_activeGroup;		// �˻� �� ��ü ���̺��� ��ȸ�� �ʿ� ����
		std::vector<IUIObject*> m_inactiveGroup;	// ��Ȱ��ȭ�� ������Ʈ�� (�˻� ��󿡼� ����)
		std::vector<IUIObject*> m_handleTable;
		IUIObject* m_pLPressedObject;
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
		// REAL_ROOT �÷��� Ȱ��ȭ �� Root Vector�� �߰��� SceneManager���� �� ���� �� �����Ѵ�.
		UIOBJECT_FLAG flag =
			static_cast<UIOBJECT_FLAG>(static_cast<uint16_t>(UIOBJECT_FLAG::PENDING) | static_cast<uint16_t>(UIOBJECT_FLAG::ACTIVE));

		// PENDING GAME OBJECT
		T* pNewUIObject = new T(this->AssignUniqueId(), flag, name);
		*ppNewUIObject = pNewUIObject;
		return this->RegisterToHandleTable(pNewUIObject);
	}
}
