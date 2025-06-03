#pragma once

#include <ZergEngine\Common\EngineHelper.h>
#include <ZergEngine\Common\EngineConstants.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class MonoBehaviour;

	class RuntimeImpl
	{
		friend class SceneManagerImpl;
		ZE_DECLARE_SINGLETON(RuntimeImpl);
	public:
		void Run(HINSTANCE hInstance, int nShowCmd, PCWSTR wndTitle, PCSTR startScene, uint32_t width, uint32_t height, WINDOW_MODE mode);
		void Exit();

		void EnableRendering();
		void DisableRendering();

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 빈 게임 오브젝트를 생성합니다.
		GameObjectHandle CreateGameObject(PCWSTR name = L"New Game Object");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 패널 UI를 생성합니다.
		UIObjectHandle CreatePanel(PCWSTR name = L"New Panel");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 이미지 UI를 생성합니다.
		UIObjectHandle CreateImage(PCWSTR name = L"New Image");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 버튼 UI를 생성합니다.
		UIObjectHandle CreateButton(PCWSTR name = L"New Button");


		void DontDestroyOnLoad(GameObjectHandle gameObject);
		void DontDestroyOnLoad(UIObjectHandle uiObject);

		void Destroy(GameObjectHandle hGameObject);
		void Destroy(GameObjectHandle hGameObject, float delay);

		void Destroy(UIObjectHandle hUIObject);
		void Destroy(UIObjectHandle hUIObject, float delay);

		void Destroy(MonoBehaviour* pScript);

		template<typename T>
		void Destroy(ComponentHandle<T> hComponent);
		template<typename T>
		void Destroy(ComponentHandle<T> hComponent, float delay);

		GameObjectHandle Instantiate(const GameObjectHandle source);
		// GameObjectHandle Instantiate(const GameObjectHandle source, GameObjectHandle pParent);

		inline HINSTANCE GetInstanceHandle() const { return m_hInstance; }
		inline PCSTR GetStartSceneName() const { return m_startScene.c_str(); }
	private:
		void InitAllSubsystem(PCWSTR wndTitle, uint32_t width, uint32_t height, WINDOW_MODE mode);
		void ReleaseAllSubsystem();
		void OnIdle();

		void Destroy(GameObject* pGameObject);
		void Destroy(GameObject* pGameObject, float delay);

		void Destroy(IUIObject* pUIObject);
		void Destroy(IUIObject* pUIObject, float delay);

		void Destroy(IComponent* pComponent);
		void Destroy(IComponent* pComponent, float delay);

		void DestroyAllComponents(GameObject* pGameObject);
		void RemoveDestroyedComponentsAndObjects();

		UIObjectHandle RegisterRootUIObject(IUIObject* pUIObject);
	private:
		HINSTANCE m_hInstance;
		std::string m_startScene;
		LONGLONG m_deltaPC;
		uint32_t m_flag;
	};

	template<typename T>
	void RuntimeImpl::Destroy(ComponentHandle<T> hComponent)
	{
		T* pComponent = hComponent.ToPtr();
		if (!pComponent)
			return;

		this->Destroy(pComponent);
	}

	extern RuntimeImpl Runtime;
}
