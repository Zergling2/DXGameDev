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
			uint32_t m_width;
			uint32_t m_height;
			bool m_fullscreen;
			PCWSTR m_title;
		};
	public:
		inline HWND GetWindowHandle() const { return m_hWnd; }
		inline uint32_t GetWidth() const { return m_width; }
		inline uint32_t GetHeight() const { return m_height; }
		inline bool IsFullscreen() const { return m_fullscreen; }

		void SetResolution(uint32_t width, uint32_t height, bool fullscreen);
		inline const D3D11_VIEWPORT& GetFullClientViewport() const { return m_fullClientViewport; }
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

        static LRESULT WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		HWND m_hWnd;
		DWORD m_style;
		uint32_t m_width;
		uint32_t m_height;
		bool m_fullscreen;
		D3D11_VIEWPORT m_fullClientViewport;
	};

	extern WindowImpl Window;
}
