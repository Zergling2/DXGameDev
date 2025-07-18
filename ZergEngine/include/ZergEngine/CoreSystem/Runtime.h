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

		// mode�� DISPLAY_MODE::BORDERLESS_WINDOWED�� ��� width, height�� ���õ˴ϴ�.
		// mode�� DISPLAY_MODE::FULLSCREEN�� ��� width, height�� ���� ��üȭ�� �ػ��� �ʺ�, ���̰��� �˴ϴ�.
		bool SetResolution(uint32_t width, uint32_t height, DISPLAY_MODE mode);

		// (�� �Լ��� ��ũ��Ʈ������ ȣ���ؾ� �մϴ�.)
		// �� ���� ������Ʈ�� �����մϴ�.
		GameObjectHandle CreateGameObject(PCWSTR name = L"New Game Object");

		// (�� �Լ��� ��ũ��Ʈ������ ȣ���ؾ� �մϴ�.)
		// ��Ʈ ������Ʈ�� �г� UI�� �����մϴ�.
		UIObjectHandle CreatePanel(PCWSTR name = L"New Panel");

		// (�� �Լ��� ��ũ��Ʈ������ ȣ���ؾ� �մϴ�.)
		// ��Ʈ ������Ʈ�� �̹��� UI�� �����մϴ�.
		UIObjectHandle CreateImage(PCWSTR name = L"New Image");

		// (�� �Լ��� ��ũ��Ʈ������ ȣ���ؾ� �մϴ�.)
		// ��Ʈ ������Ʈ�� ��ư UI�� �����մϴ�.
		UIObjectHandle CreateButton(PCWSTR name = L"New Button");

		GameObjectHandle Instantiate(const GameObjectHandle source);
		// GameObjectHandle Instantiate(const GameObjectHandle source, GameObjectHandle parent);

		// ��������������������������������������������������������������������������������������������������������������������
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
		// ��������������������������������������������������������������������������������������������������������������������

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
