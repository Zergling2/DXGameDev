#pragma once

#include <ZergEngine\Common\EngineHeaders.h>

namespace zergengine
{
	enum class Axis
	{
		HORIZONTAL,
		VERTICAL
	};

	class Input
	{
		friend class Engine;
	private:
		static sysresult SysStartup(const EngineSystemInitDescriptor& desc);
		static void SysCleanup();

		static void Update();
	public:
		static inline POINT GetMousePosition() { return Input::s_mousePos; }
	private:
		static ComPtr<IDirectInput8> s_directInput;
		static ComPtr<IDirectInputDevice8> s_keyboard;
		static ComPtr<IDirectInputDevice8> s_mouse;
		static uint8_t s_keyboardState[256];
		static DIMOUSESTATE s_mouseState;
		static POINT s_mousePos;
	};
}
