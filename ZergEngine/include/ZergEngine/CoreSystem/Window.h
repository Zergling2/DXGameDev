#pragma once

#include <ZergEngine\Common\ThirdPartySDK.h>

namespace ze
{
	class IWindowMessageHandler
	{
	public:
		IWindowMessageHandler() = default;
		virtual ~IWindowMessageHandler() = default;

		virtual void OnCreate(WPARAM wParam, LPARAM lParam) {}
		virtual void OnDestroy(WPARAM wParam, LPARAM lParam) {}
		virtual void OnSize(UINT nType, int cx, int cy) {}
		virtual void OnMove(WPARAM wParam, LPARAM lParam) {}
		virtual void OnKillFocus(WPARAM wParam, LPARAM lParam) {};
		virtual void OnSetFocus(WPARAM wParam, LPARAM lParam) {};
		virtual void OnShowWindow(WPARAM wParam, LPARAM lParam) {}
		virtual void OnActivateApp(WPARAM wParam, LPARAM lParam) {};
		virtual void OnChar(WPARAM wParam, LPARAM lParam) {}
		virtual void OnMouseMove(UINT flags, POINT pt) {}
		virtual void OnLButtonDown(UINT flags, POINT pt) {}
		virtual void OnLButtonUp(UINT flags, POINT pt) {}
		virtual void OnRButtonDown(UINT flags, POINT pt) {}
		virtual void OnRButtonUp(UINT flags, POINT pt) {}
		virtual void OnMButtonDown(UINT flags, POINT pt) {}
		virtual void OnMButtonUp(UINT flags, POINT pt) {}
		virtual void OnMoving(WPARAM wParam, LPARAM lParam) {}
		virtual void OnEnterSizeMove(WPARAM wParam, LPARAM lParam) {}
		virtual void OnExitSizeMove(WPARAM wParam, LPARAM lParam) {}
	};

	class Window
	{
	public:
		Window();
		~Window();

		void Create(IWindowMessageHandler* pHandler, HINSTANCE hInstance, DWORD style, uint32_t width, uint32_t height, PCWSTR title);
		void Destroy();

		HWND GetHandle() const { return m_hWnd; }
		BOOL GetWindowRect(LPRECT pRect) const { ::GetWindowRect(m_hWnd, pRect); }
		BOOL GetClientRect(LPRECT pRect) const { ::GetClientRect(m_hWnd, pRect); }
		LONG GetClientWidth() const { return m_clientSize.cx; }
		LONG GetClientHeight() const { return m_clientSize.cy; }

		DWORD GetStyle() const;
		void SetStyle(DWORD style);

		BOOL ShowWindow(int nCmdShow);

		BOOL Resize(uint32_t width, uint32_t height);

        static LRESULT CALLBACK WinProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		IWindowMessageHandler* m_pHandler;
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		SIZE m_clientSize;
	};
}
