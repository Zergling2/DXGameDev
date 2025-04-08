#pragma once

#include <ZergEngine\Common\EngineHelper.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class SyncFileLogger;
	class IScript;

	class RuntimeImpl
	{
		ZE_DECLARE_SINGLETON(RuntimeImpl);
	public:
		void Run(HINSTANCE hInstance, int nShowCmd, PCWSTR wndTitle, PCWSTR startScene, uint32_t width, uint32_t height, bool fullscreen);
		void Exit();

		void EnableRendering();
		void DisableRendering();

		// 스크립트에서 게임 오브젝트 생성 시 사용
		GameObjectHandle CreateGameObject(PCWSTR name = L"New Game Object");
		void DontDestroyOnLoad(GameObjectHandle gameObject);

		void Destroy(GameObjectHandle hGameObject);
		void Destroy(GameObjectHandle hGameObject, float delay);

		void DestroyAllComponents(GameObjectHandle hGameObject);

		// 올바른 매니저의 Unregister()를 호출해주고 컴포넌트 메모리 해제도 해주어야 한다.
		// 그리고 자신을 참조하는 게임오브젝트에 가서 자신을 가리키는 핸들을 제거해야 한다.
		void Destroy(ComponentHandle hComponent);
		void Destroy(ComponentHandle hComponent, float delay);

		void Destroy(IScript* pScript);

		GameObjectHandle Instantiate(const GameObjectHandle source);
		// static GameObjectHandle Instantiate(const GameObjectHandle source, GameObjectHandle pParent);

		// Find a GameObject by name.
		GameObjectHandle Find(PCWSTR name);
		GameObjectHandle Find(const std::wstring& name) { return RuntimeImpl::Find(name.c_str()); }

		inline HINSTANCE GetInstanceHandle() const { return m_hInstance; }
		inline PCWSTR GetStartSceneName() const { return m_startScene.c_str(); }
	private:
		void InitAllSubsystem(PCWSTR wndTitle, uint32_t width, uint32_t height, bool fullscreen);
		void ReleaseAllSubsystem();
		void OnIdle();
	private:
		HINSTANCE m_hInstance;
		std::wstring m_startScene;
		float m_fixedUpdateTimer;
		uint32_t m_flag;
	};

	extern RuntimeImpl Runtime;
}
