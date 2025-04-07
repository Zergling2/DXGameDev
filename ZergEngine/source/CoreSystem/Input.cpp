#include <ZergEngine\CoreSystem\Input.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\Window.h>

using namespace ze;

InputImpl ze::Input;

static PCWSTR UPDATE_LOG_PREFIX = L"InputImpl::Update > ";

InputImpl::InputImpl()
	: m_cpDirectInput(nullptr)
	, m_cpDIKeyboard(nullptr)
	, m_cpDIMouse(nullptr)
	, m_prevKeyState{ 0 }
	, m_currKeyState{ 0 }
	, m_mouseState{ 0 }
	, m_mousePos{ 0, 0 }
{
}

InputImpl::~InputImpl()
{
}

void InputImpl::Init(void* pDesc)
{
	HRESULT hr;

	ZeroMemory(&m_prevKeyState, sizeof(m_prevKeyState));
	ZeroMemory(&m_currKeyState, sizeof(m_currKeyState));
	GetCursorPos(&m_mousePos);

	// DirectInput 초기화
	hr = DirectInput8Create(
		Runtime.GetInstanceHandle(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<LPVOID*>(m_cpDirectInput.GetAddressOf()),
		nullptr
	);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"InputImpl::Init() > DirectInput8Create() failed.", hr);

	// Keyboard 인터페이스 생성
	hr = m_cpDirectInput->CreateDevice(GUID_SysKeyboard, m_cpDIKeyboard.GetAddressOf(), nullptr);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"InputImpl::Init() > m_cpDirectInput->CreateDevice() failed.", hr);

	hr = m_cpDIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"InputImpl::Init() > m_cpDIKeyboard->SetDataFormat() failed.", hr);

	hr = m_cpDIKeyboard->SetCooperativeLevel(Window.GetWindowHandle(), DISCL_FOREGROUND | DISCL_EXCLUSIVE);	// DISCL_EXCLUSIVE
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"InputImpl::Init() > m_cpDIKeyboard->SetCooperativeLevel() failed.", hr);

	hr = m_cpDIKeyboard->Acquire();

	// Mouse 인터페이스 생성
	hr = m_cpDirectInput->CreateDevice(GUID_SysMouse, m_cpDIMouse.GetAddressOf(), nullptr);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"InputImpl::Init() > m_cpDirectInput->CreateDevice() failed.", hr);

	hr = m_cpDIMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"InputImpl::Init() > m_cpDIMouse->SetDataFormat() failed.", hr);

	hr = m_cpDIMouse->SetCooperativeLevel(Window.GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);	// DISCL_NONEXCLUSIVE
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"InputImpl::Init() > m_cpDIMouse->SetCooperativeLevel() failed.", hr);

	hr = m_cpDIMouse->Acquire();
}

void InputImpl::Release()
{
	if (m_cpDIMouse)
		m_cpDIMouse->Unacquire();

	if (m_cpDIKeyboard)
		m_cpDIKeyboard->Unacquire();

	m_cpDIMouse.Reset();
	m_cpDIKeyboard.Reset();
	m_cpDirectInput.Reset();
}

void InputImpl::Update()
{
	HRESULT hr;

	// Read keyboard
	hr = m_cpDIKeyboard->GetDeviceState(sizeof(m_currKeyState), reinterpret_cast<LPVOID>(m_currKeyState));
	if (FAILED(hr) && (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED))
	{
		hr = m_cpDIKeyboard->Acquire();
		if (SUCCEEDED(hr))
			m_cpDIKeyboard->GetDeviceState(sizeof(m_currKeyState), reinterpret_cast<LPVOID>(m_currKeyState));
	}

	// Read mouse
	m_mouseState.lX = 0;
	m_mouseState.lY = 0;
	hr = m_cpDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), reinterpret_cast<LPVOID>(&m_mouseState));
	if (FAILED(hr) && (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED))
	{
		hr = m_cpDIMouse->Acquire();
		if (SUCCEEDED(hr))
			m_cpDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), reinterpret_cast<LPVOID>(&m_mouseState));
	}

	m_mousePos.x += m_mouseState.lX;
	m_mousePos.y += m_mouseState.lY;
	Math::Clamp(m_mousePos.x, 0L, static_cast<LONG>(Window.GetWidth()));
	Math::Clamp(m_mousePos.y, 0L, static_cast<LONG>(Window.GetHeight()));
}

void InputImpl::FinalUpdate()
{
	memcpy(m_prevKeyState, m_currKeyState, sizeof(m_prevKeyState));
}

bool InputImpl::GetKey(uint8_t keyCode)
{
	return m_currKeyState[keyCode] & 0x80;
}

bool InputImpl::GetKeyDown(uint8_t keyCode)
{
	return (m_currKeyState[keyCode] & 0x80) && !(m_prevKeyState[keyCode] & 0x80);
}

bool InputImpl::GetKeyUp(uint8_t keyCode)
{
	return !(m_currKeyState[keyCode] & 0x80) && (m_prevKeyState[keyCode] & 0x80);
}
