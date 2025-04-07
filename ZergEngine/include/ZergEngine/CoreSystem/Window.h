#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	class WindowImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(WindowImpl);
		struct InitDesc
		{
			SIZE m_resolution;
			bool m_fullscreen;
			PCWSTR m_title;
		};
	public:
		inline HWND GetWindowHandle() const { return m_hWnd; }
		inline uint32_t GetWidth() const { return static_cast<uint32_t>(m_resolution.cx); }
		inline uint32_t GetHeight() const { return static_cast<uint32_t>(m_resolution.cy); }
		inline bool IsFullscreen() const { return m_fullscreen; }

		void SetResolution(SIZE resolution, bool fullscreen);
		inline const D3D11_VIEWPORT& GetFullClientViewport() const { return m_fullClientViewport; }
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

        static LRESULT WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		static void OnResize(SIZE resolution);
	private:
		HWND m_hWnd;
		SIZE m_resolution;
		bool m_fullscreen;
		D3D11_VIEWPORT m_fullClientViewport;
	};

	extern WindowImpl Window;
}
