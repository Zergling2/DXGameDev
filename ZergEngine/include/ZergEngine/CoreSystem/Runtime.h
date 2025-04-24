#pragma once

#include <ZergEngine\Common\EngineHelper.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class SyncFileLogger;
	class IScript;

	class RuntimeImpl
	{
		friend class SceneManagerImpl;
		ZE_DECLARE_SINGLETON(RuntimeImpl);
	public:
		void Run(HINSTANCE hInstance, int nShowCmd, PCWSTR wndTitle, PCSTR startScene, uint32_t width, uint32_t height, bool fullscreen);
		void Exit();

		void EnableRendering();
		void DisableRendering();

		// 스크립트에서 게임 오브젝트 생성 시 사용
		GameObjectHandle CreateGameObject(PCWSTR name = L"New Game Object");
		GameObjectHandle CreateCanvasItemObject(PCWSTR name = L"New Canvas Item");
		void DontDestroyOnLoad(GameObjectHandle gameObject);

		void Destroy(GameObjectHandle hGameObject);
		void Destroy(GameObjectHandle hGameObject, float delay);

		void DestroyAllComponents(GameObjectHandle hGameObject);

		template<typename _T>
		void Destroy(ComponentHandle<_T> hComponent);
		template<typename _T>
		void Destroy(ComponentHandle<_T> hComponent, float delay);

		void Destroy(IScript* pScript);

		GameObjectHandle Instantiate(const GameObjectHandle source);
		// GameObjectHandle Instantiate(const GameObjectHandle source, GameObjectHandle pParent);

		// Find a GameObject by name.
		GameObjectHandle Find(PCWSTR name);
		GameObjectHandle Find(const std::wstring& name) { return RuntimeImpl::Find(name.c_str()); }

		inline HINSTANCE GetInstanceHandle() const { return m_hInstance; }
		inline PCSTR GetStartSceneName() const { return m_startScene.c_str(); }
	private:
		void InitAllSubsystem(PCWSTR wndTitle, uint32_t width, uint32_t height, bool fullscreen);
		void ReleaseAllSubsystem();
		void OnIdle();

		void Destroy(GameObject* pGameObject);
		void Destroy(GameObject* pGameObject, float delay);

		void Destroy(IComponent* pComponent);
		void Destroy(IComponent* pComponent, float delay);

		void DestroyAllComponents(GameObject* pGameObject);

		void RemoveDestroyedComponentsAndGameObjects();
	private:
		HINSTANCE m_hInstance;
		std::string m_startScene;
		float m_fixedUpdateTimer;
		uint32_t m_flag;
	};

	template<typename _T>
	void RuntimeImpl::Destroy(ComponentHandle<_T> hComponent)
	{
		_T* pComponent = hComponent.ToPtr();
		if (!pComponent)
			return;

		this->Destroy(pComponent);
	}

	extern RuntimeImpl Runtime;
}
