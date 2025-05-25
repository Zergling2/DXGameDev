#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\Common\EngineConstants.h>

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
			WINDOW_MODE m_mode;
			PCWSTR mTitle;
		};
	public:
		HWND GetWindowHandle() const { return m_hWnd; }
		uint32_t GetWidthInteger() const { return m_width; }
		uint32_t GetHeightInteger() const { return m_height; }
		float GetWidthFloat() const { return m_sizeFlt.x; }
		float GetHeightFloat() const { return m_sizeFlt.y; }
		float GetHalfWidthFloat() const { return m_halfSizeFlt.x; }
		float GetHalfHeightFloat() const { return m_halfSizeFlt.y; }
		bool IsFullscreen() const { return m_mode == WINDOW_MODE::FULLSCREEN; }

		void SetResolution(uint32_t width, uint32_t height, WINDOW_MODE mode);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

        static LRESULT WinProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	private:
		HWND m_hWnd;
		DWORD m_style;
		uint32_t m_width;
		uint32_t m_height;
		XMFLOAT2 m_sizeFlt;
		XMFLOAT2 m_halfSizeFlt;
		WINDOW_MODE m_mode;
	};

	extern WindowImpl Window;
}
