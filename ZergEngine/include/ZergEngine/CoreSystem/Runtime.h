#pragma once

#include <ZergEngine\Common\DisplayMode.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class IScene;

	class Runtime : public IWindowMessageHandler
	{
	public:
		Runtime();
		virtual ~Runtime();

		static void CreateInstance();
		static void DestroyInstance();
		static Runtime* GetInstance() { return s_pInstance; }

		void Init(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height, PCWSTR title, PCWSTR startScene);
		void UnInit();

		void Run();
		void Exit();
		void OnIdle();

		// mode가 DISPLAY_MODE::BORDERLESS_WINDOWED일 경우 width, height는 무시됩니다.
		// mode가 DISPLAY_MODE::FULLSCREEN일 경우 width, height는 각각 전체화면 해상도의 너비, 높이값이 됩니다.
		bool SetResolution(uint32_t width, uint32_t height, DISPLAY_MODE mode);

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

		GameObjectHandle Instantiate(const GameObjectHandle source);
		// GameObjectHandle Instantiate(const GameObjectHandle source, GameObjectHandle parent);

		HINSTANCE GetInstanceHandle() const { return m_hInstance; }
	private:
		void LoadNextScene(IScene* pNextScene);
		void DestroyAllObjectExceptDontDestroyOnLoad();
		void DestroyAllObject();
		void RemoveDestroyedComponentsAndObjects();

		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		// Window Message Handler
		// virtual void OnCreate(WPARAM wParam, LPARAM lParam) override;
		// virtual void OnDestroy(WPARAM wParam, LPARAM lParam) override;
		virtual void OnSize(WPARAM wParam, LPARAM lParam) override;
		virtual void OnShowWindow(WPARAM wParam, LPARAM lParam) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnMouseMove(WPARAM wParam, LPARAM lParam) override;
		virtual void OnLButtonDown(WPARAM wParam, LPARAM lParam) override;
		virtual void OnLButtonUp(WPARAM wParam, LPARAM lParam) override;
		virtual void OnRButtonDown(WPARAM wParam, LPARAM lParam) override {}
		virtual void OnRButtonUp(WPARAM wParam, LPARAM lParam) override {}
		virtual void OnMButtonDown(WPARAM wParam, LPARAM lParam) override {}
		virtual void OnMButtonUp(WPARAM wParam, LPARAM lParam) override {}
		virtual void OnEnterSizeMove(WPARAM wParam, LPARAM lParam) override;
		virtual void OnExitSizeMove(WPARAM wParam, LPARAM lParam) override;
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
	protected:
		static Runtime* s_pInstance;
	private:
		HINSTANCE m_hInstance;
		int m_nCmdShow;
		Window m_window;
		LONGLONG m_deltaPerformanceCount;
		bool m_render;
	};
}
