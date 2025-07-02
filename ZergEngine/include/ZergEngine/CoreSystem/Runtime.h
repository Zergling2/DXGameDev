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

		HINSTANCE GetInstanceHandle() const { return m_hInstance; }
	private:
		void LoadNextScene(IScene* pNextScene);
		void DestroyAllObjectExceptDontDestroyOnLoad();
		void DestroyAllObject();
		void RemoveDestroyedComponentsAndObjects();

		// ��������������������������������������������������������������������������������������������������������������������
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
		// ��������������������������������������������������������������������������������������������������������������������
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
