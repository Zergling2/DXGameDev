#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	class GameObject;
	class IComponent;

	class Engine
	{
		enum WINDOW_STATE_FLAG : uint32_t
		{
			NORMAL = 0x00000000,
			IS_RESIZING = 0x00000001
		};
	public:
		static sysresult Start(HINSTANCE hInstance, int nShowCmd, LPCWSTR wndTitle, const SceneTableMap& stm, LPCWSTR startSceneName,
			uint32_t width, uint32_t height, bool fullscreen);
		static void Exit();

		static inline const SYSTEM_INFO& GetSystemInfo() { return Engine::s_sysInfo; }
		static inline LPCWSTR GetStartSceneName() { return Engine::s_startSceneName.c_str(); }
		static inline HINSTANCE GetInstanceHandle() { return Engine::s_hInstance; }
		static inline HWND GetMainWindowHandle() { return Engine::s_hMainWnd; }
	private:
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static sysresult Startup(HINSTANCE hInstance, LPCWSTR wndTitle, const SceneTableMap& stm, LPCWSTR startSceneName,
			uint32_t width, uint32_t height, bool fullscreen);
		static void RuntimeLoop();
		static void Cleanup();

		static inline uint32_t GetWindowStateFlag() { return Engine::s_wndStateFlag; }
		static inline void SetWindowStateFlag(uint32_t flag) { Engine::s_wndStateFlag |= flag; }
		static inline void UnsetWindowStateFlag(uint32_t flag) { Engine::s_wndStateFlag &= (~flag); }
	private:
		static bool s_coInited;
		static SYSTEM_INFO s_sysInfo;
		static uint32_t s_wndStateFlag;
		static HINSTANCE s_hInstance;
		static std::wstring s_startSceneName;
		static HWND s_hMainWnd;
		static float s_loopTime;
	};

	void DontDestroyOnLoad(std::shared_ptr<GameObject>& gameObject);
	void Destroy(std::shared_ptr<GameObject>& gameObject);
	// void Destroy(std::shared_ptr<GameObject>& gameObject, float delay);
	void Destroy(std::shared_ptr<IComponent>& component);
	// void Destroy(std::shared_ptr<IComponent>& component, float delay);
	// std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<GameObject>& original);
	// std::shared_ptr<GameObject> Instantiate(const std::shared_ptr<GameObject>& original, std::shared_ptr<GameObject>& parent);
}
