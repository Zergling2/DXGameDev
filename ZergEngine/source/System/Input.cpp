#include <ZergEngine\System\Input.h>
#include <ZergEngine\System\Runtime.h>
#include <ZergEngine\System\Debug.h>
#include <ZergEngine\System\Graphics.h>
#include <ZergEngine\Common\EngineMath.h>

using namespace zergengine;

static LPCWSTR const THIS_FILE_NAME = L"Input.cpp";

ComPtr<IDirectInput8> Input::s_directInput;
ComPtr<IDirectInputDevice8> Input::s_keyboard;
ComPtr<IDirectInputDevice8> Input::s_mouse;
uint8_t Input::s_keyboardState[256];
DIMOUSESTATE Input::s_mouseState;
POINT Input::s_mousePos = { 0, 0 };

sysresult Input::SysStartup(const EngineSystemInitDescriptor& desc)
{
	// initialize...
	HRESULT hr;

	ZeroMemory(&Input::s_keyboardState, sizeof(Input::s_keyboardState));

	GetCursorPos(&Input::s_mousePos);

	// DirectInput 초기화
	hr = DirectInput8Create(
		Engine::GetInstanceHandle(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<LPVOID*>(Input::s_directInput.GetAddressOf()),
		nullptr
	);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	// Keyboard 인터페이스 생성
	hr = Input::s_directInput->CreateDevice(GUID_SysKeyboard, &Input::s_keyboard, nullptr);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	hr = Input::s_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	hr = Input::s_keyboard->SetCooperativeLevel(Engine::GetMainWindowHandle(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);	// DISCL_EXCLUSIVE
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	hr = Input::s_keyboard->Acquire();
	/*
	if (FAILED(hr))
	{
		if (hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED)
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
		}
	}
	*/

	// Mouse 인터페이스 생성
	hr = Input::s_directInput->CreateDevice(GUID_SysMouse, &Input::s_mouse, nullptr);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	hr = Input::s_mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	hr = Input::s_mouse->SetCooperativeLevel(Engine::GetMainWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);	// DISCL_NONEXCLUSIVE
	if (FAILED(hr))
	{
		hrlog(THIS_FILE_NAME, __LINE__, hr);
		return -1;
	}

	hr = Input::s_mouse->Acquire();
	/*
	if (FAILED(hr))
	{
		if (hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED)
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
		}
	}
	*/

	return 0;
}

void Input::SysCleanup()
{
	Input::s_mouse->Unacquire();
	Input::s_keyboard->Unacquire();

	Input::s_mouse.Reset();
	Input::s_keyboard.Reset();
	Input::s_directInput.Reset();
}

void Input::Update()
{
	HRESULT hr;

	// Read keyboard
	hr = Input::s_keyboard->GetDeviceState(
		sizeof(Input::s_keyboardState),
		reinterpret_cast<LPVOID>(Input::s_keyboardState)
	);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			hr = Input::s_keyboard->Acquire();
			/*
			if (FAILED(hr))
			{
				hrlog(THIS_FILE_NAME, __LINE__, hr);
			}
			*/
		}
		else
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
		}
	}

	// Read mouse
	Input::s_mouseState.lX = 0;
	Input::s_mouseState.lY = 0;
	hr = Input::s_mouse->GetDeviceState(
		sizeof(DIMOUSESTATE),
		reinterpret_cast<LPVOID>(&Input::s_mouseState)
	);
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			hr = Input::s_mouse->Acquire();
			/*
			if (FAILED(hr))
			{
				hrlog(THIS_FILE_NAME, __LINE__, hr);
			}
			*/
		}
		else
		{
			hrlog(THIS_FILE_NAME, __LINE__, hr);
		}
	}

	// Process input
	Input::s_mousePos.x += Input::s_mouseState.lX;
	Input::s_mousePos.y += Input::s_mouseState.lY;

	Math::Clamp(Input::s_mousePos.x, 0L, static_cast<LONG>(Screen::GetWidth()));
	Math::Clamp(Input::s_mousePos.y, 0L, static_cast<LONG>(Screen::GetHeight()));
}
