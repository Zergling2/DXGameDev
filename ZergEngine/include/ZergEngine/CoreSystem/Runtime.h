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
		virtual ~Runtime() = default;

		static void CreateInstance();
		static void DestroyInstance();
		static Runtime* GetInstance() { return s_pInstance; }

		void Init(HINSTANCE hInstance, int nCmdShow, uint32_t width, uint32_t height, PCWSTR title, PCWSTR startScene);
		void InitEditor(HINSTANCE hInstance, HWND hMainFrameWnd, HWND hViewWnd, uint32_t width, uint32_t height);
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

		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		// Window Message Handler
		// virtual void OnCreate(WPARAM wParam, LPARAM lParam) override;
		// virtual void OnDestroy(WPARAM wParam, LPARAM lParam) override;
		virtual void OnSize(UINT nType, int cx, int cy) override;
		virtual void OnChar(WPARAM wParam, LPARAM lParam) override;
		virtual void OnMouseMove(UINT flags, POINT pt) override;
		virtual void OnLButtonDown(UINT flags, POINT pt) override;
		virtual void OnLButtonUp(UINT flags, POINT pt) override;
		virtual void OnRButtonDown(UINT flags, POINT pt) override;
		virtual void OnRButtonUp(UINT flags, POINT pt) override;
		virtual void OnMButtonDown(UINT flags, POINT pt) override;
		virtual void OnMButtonUp(UINT flags, POINT pt) override;
		virtual void OnEnterSizeMove(WPARAM wParam, LPARAM lParam) override;
		virtual void OnExitSizeMove(WPARAM wParam, LPARAM lParam) override;
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

		void DestroyAllObjectExceptDontDestroyOnLoad();
		void DestroyAllObject();
		void RemoveDestroyedComponentsAndObjects();
	protected:
		void OutputDXGIDebugLog() const;
	private:
		void LoadNextScene(IScene* pNextScene);
	private:
		static Runtime* s_pInstance;
		LONGLONG m_deltaPerformanceCount;
		bool m_isEditor;
		bool m_render;
		int m_nCmdShow;
		Window m_window;
	};
}
