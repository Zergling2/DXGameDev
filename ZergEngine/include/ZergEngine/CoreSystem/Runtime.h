#pragma once

#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\DisplayMode.h>
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
		bool IsEditor() const { return m_isEditor; }
		HWND GetGameWindowHandle() const { return m_hGameWnd; }
		SyncFileLogger& GetSyncFileLogger() { return m_sfl; }
		AsyncFileLogger& GetAsyncFileLogger() { return m_afl; }

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 빈 게임 오브젝트를 생성합니다.
		GameObjectHandle CreateGameObject(PCWSTR name = L"New Game Object");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 패널을 생성합니다.
		UIObjectHandle CreatePanel(PCWSTR name = L"New Panel");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 이미지를 생성합니다.
		UIObjectHandle CreateImage(PCWSTR name = L"New Image");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 텍스트를 생성합니다.
		UIObjectHandle CreateText(PCWSTR name = L"New Text");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 버튼을 생성합니다.
		UIObjectHandle CreateButton(PCWSTR name = L"New Button");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 입력 필드를 생성합니다.
		UIObjectHandle CreateInputField(PCWSTR name = L"New Input Field");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 슬라이더 컨트롤을 생성합니다.
		UIObjectHandle CreateSliderControl(PCWSTR name = L"New Slider Control");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 체크박스를 생성합니다.
		UIObjectHandle CreateCheckbox(PCWSTR name = L"New Checkbox");

		// (이 함수는 스크립트에서만 호출해야 합니다.)
		// 루트 오브젝트로 라디오 버튼을 생성합니다.
		UIObjectHandle CreateRadioButton(PCWSTR name = L"New Radio Button");

		GameObjectHandle Instantiate(const GameObjectHandle source);
		// GameObjectHandle Instantiate(const GameObjectHandle source, GameObjectHandle parent);
		
		// ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
		// Window Message Handler
		// virtual void OnCreate(WPARAM wParam, LPARAM lParam) override;
		virtual void OnDestroy(WPARAM wParam, LPARAM lParam) override;
		virtual void OnSize(UINT nType, int cx, int cy) override;
		virtual void OnMove(WPARAM wParam, LPARAM lParam) override;
		virtual void OnActivateApp(WPARAM wParam, LPARAM lParam) override;
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
		bool __$$SetResolutionImpl(uint32_t width, uint32_t height, DisplayMode mode);
	protected:
		void OutputDXGIDebugLog() const;
	private:
		void CreateLoggers();
		void ReleaseLoggers();
		void LoadNextScene(IScene* pNextScene);
	private:
		static Runtime* s_pInstance;
		LONGLONG m_deltaPerformanceCount;
		bool m_isEditor;
		bool m_render;
		int m_nCmdShow;
		Window m_window;
		HWND m_hGameWnd;
		SyncFileLogger m_sfl;
		AsyncFileLogger m_afl;
	};
}
