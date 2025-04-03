#include <ZergEngine\CoreSystem\Input.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\Window.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(Input);

static PCWSTR UPDATE_LOG_PREFIX = L"Input::Update > ";

Input::Input()
	: m_cpDirectInput(nullptr)
	, m_cpDIKeyboard(nullptr)
	, m_cpDIMouse(nullptr)
	, m_keyboardState{0}
	, m_mouseState{0}
	, m_mousePos{ 0, 0 }
{
}

Input::~Input()
{
}

void Input::Init(void* pDesc)
{
	HRESULT hr;

	ZeroMemory(&m_keyboardState, sizeof(m_keyboardState));
	GetCursorPos(&m_mousePos);

	// DirectInput 초기화
	hr = DirectInput8Create(
		Runtime::GetInstanceHandle(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<LPVOID*>(m_cpDirectInput.GetAddressOf()),
		nullptr
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > DirectInput8Create() failed.", hr);

	// Keyboard 인터페이스 생성
	hr = m_cpDirectInput->CreateDevice(GUID_SysKeyboard, m_cpDIKeyboard.GetAddressOf(), nullptr);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > m_cpDirectInput->CreateDevice() failed.", hr);

	hr = m_cpDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > m_cpDIKeyboard->SetDataFormat() failed.", hr);

	hr = m_cpDIKeyboard->SetCooperativeLevel(Window::GetInstance().GetWindowHandle(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);	// DISCL_EXCLUSIVE
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > m_cpDIKeyboard->SetCooperativeLevel() failed.", hr);

	hr = m_cpDIKeyboard->Acquire();

	// Mouse 인터페이스 생성
	hr = m_cpDirectInput->CreateDevice(GUID_SysMouse, m_cpDIMouse.GetAddressOf(), nullptr);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > m_cpDirectInput->CreateDevice() failed.", hr);

	hr = m_cpDIMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > m_cpDIMouse->SetDataFormat() failed.", hr);

	hr = m_cpDIMouse->SetCooperativeLevel(Window::GetInstance().GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);	// DISCL_NONEXCLUSIVE
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > m_cpDIMouse->SetCooperativeLevel() failed.", hr);

	hr = m_cpDIMouse->Acquire();
}

void Input::Release()
{
	if (m_cpDIMouse)
		m_cpDIMouse->Unacquire();

	if (m_cpDIKeyboard)
		m_cpDIKeyboard->Unacquire();

	m_cpDIMouse.Reset();
	m_cpDIKeyboard.Reset();
	m_cpDirectInput.Reset();
}

void Input::Update()
{
	HRESULT hr;

	// Read keyboard
	hr = m_cpDIKeyboard->GetDeviceState(sizeof(m_keyboardState), reinterpret_cast<LPVOID>(m_keyboardState));
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			hr = m_cpDIKeyboard->Acquire();
			if (SUCCEEDED(hr))
				m_cpDIKeyboard->GetDeviceState(sizeof(m_keyboardState), reinterpret_cast<LPVOID>(m_keyboardState));
		}
	}

	// Read mouse
	m_mouseState.lX = 0;
	m_mouseState.lY = 0;
	hr = m_cpDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), reinterpret_cast<LPVOID>(&m_mouseState));
	if (FAILED(hr))
	{
		if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED)
		{
			hr = m_cpDIMouse->Acquire();
			if (SUCCEEDED(hr))
			{
				m_cpDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), reinterpret_cast<LPVOID>(&m_mouseState));
			}
		}
	}

	GetCursorPos(&m_mousePos);
}
