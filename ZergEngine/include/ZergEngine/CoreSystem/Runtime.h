#pragma once

#include <ZergEngine\Common\EngineHelper.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class SyncFileLogger;
	class IScript;

	class Runtime
	{
		ZE_DECLARE_SINGLETON(Runtime);
	public:
		enum RUNTIME_FLAG : uint32_t
		{
			RENDER_ENABLED	= 0x00000001,
			SCRIPT_UPDATE	= 0x00000002
		};
	public:
		static void Run(HINSTANCE hInstance, int nShowCmd, PCWSTR wndTitle, PCWSTR startScene, SIZE resolution, bool fullscreen);
		static void Exit();

		static void EnableRendering() { Runtime::s_flag |= RENDER_ENABLED; }
		static void DisableRendering() { Runtime::s_flag = Runtime::s_flag & ~RENDER_ENABLED; }

		// ��ũ��Ʈ���� ���� ������Ʈ ���� �� ���
		static GameObjectHandle CreateGameObject(PCWSTR name = L"New Game Object");
		static void DontDestroyOnLoad(GameObjectHandle gameObject);

		static void Destroy(GameObjectHandle hGameObject);
		static void Destroy(GameObjectHandle hGameObject, float delay);

		static void DestroyAllComponents(GameObjectHandle hGameObject);

		// �ùٸ� �Ŵ����� Unregister()�� ȣ�����ְ� ������Ʈ �޸� ������ ���־�� �Ѵ�.
		// �׸��� �ڽ��� �����ϴ� ���ӿ�����Ʈ�� ���� �ڽ��� ����Ű�� �ڵ��� �����ؾ� �Ѵ�.
		static void Destroy(ComponentHandle hComponent);
		static void Destroy(ComponentHandle hComponent, float delay);

		static void Destroy(IScript* pScript);

		static GameObjectHandle Instantiate(const GameObjectHandle source);
		// static GameObjectHandle Instantiate(const GameObjectHandle source, GameObjectHandle pParent);

		// Find a GameObject by name.
		static GameObjectHandle Find(PCWSTR name);
		static inline GameObjectHandle Find(const std::wstring& name) { return Runtime::Find(name.c_str()); }

		static inline HINSTANCE GetInstanceHandle() { return Runtime::s_hInstance; }
		static inline PCWSTR GetStartSceneName() { return Runtime::s_startScene.c_str(); }
	private:
		static void InitAllSubsystem(PCWSTR wndTitle, SIZE resolution, bool fullscreen);
		static void ReleaseAllSubsystem();
		static void OnIdle();
	private:
		static HINSTANCE s_hInstance;
		static std::wstring s_startScene;
		static float s_loopTime;
		static uint32_t s_flag;
	};
}
