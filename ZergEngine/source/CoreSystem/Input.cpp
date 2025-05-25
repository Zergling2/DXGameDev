#include <ZergEngine\CoreSystem\Input.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\Window.h>

namespace ze
{
	InputImpl Input;
}

using namespace ze;

static PCWSTR UPDATE_LOG_PREFIX = L"InputImpl::Update > ";

InputImpl::InputImpl()
	: m_cpDirectInput(nullptr)
	, m_cpDIKeyboard(nullptr)
	, m_cpDIMouse(nullptr)
	, m_prevKeyState{}
	, m_currKeyState{}
	, m_prevMouseBtnState{}
	, m_currMouseState{}
	, m_cursorPos{ 0, 0 }
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
	ZeroMemory(m_prevMouseBtnState, sizeof(m_prevMouseBtnState));
	ZeroMemory(&m_currMouseState, sizeof(m_currMouseState));
	m_cursorPos.x = 0;
	m_cursorPos.y = 0;

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

	hr = m_cpDIMouse->SetDataFormat(&c_dfDIMouse2);
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
	memcpy(m_prevKeyState, m_currKeyState, sizeof(m_prevKeyState));
	memcpy(m_prevMouseBtnState, m_currMouseState.rgbButtons, sizeof(m_prevMouseBtnState));

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
	hr = m_cpDIMouse->GetDeviceState(sizeof(m_currMouseState), reinterpret_cast<LPVOID>(&m_currMouseState));
	if (FAILED(hr) && (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED))
	{
		hr = m_cpDIMouse->Acquire();
		if (SUCCEEDED(hr))
			m_cpDIMouse->GetDeviceState(sizeof(m_currMouseState), reinterpret_cast<LPVOID>(&m_currMouseState));
	}

	m_cursorPos.x += m_currMouseState.lX;
	m_cursorPos.y += m_currMouseState.lY;
	Math::Clamp(m_cursorPos.x, 0L, static_cast<LONG>(Window.GetWidthInteger()));
	Math::Clamp(m_cursorPos.y, 0L, static_cast<LONG>(Window.GetHeightInteger()));
}

bool InputImpl::GetKey(KEYCODE code) const
{
	assert(static_cast<size_t>(code) < 256);
	return m_currKeyState[static_cast<size_t>(code)] & 0x80;
}

bool InputImpl::GetKeyDown(KEYCODE code) const
{
	assert(static_cast<size_t>(code) < 256);
	return (m_currKeyState[static_cast<size_t>(code)] & 0x80) && !(m_prevKeyState[static_cast<size_t>(code)] & 0x80);
}

bool InputImpl::GetKeyUp(KEYCODE code) const
{
	assert(static_cast<size_t>(code) < 256);
	return !(m_currKeyState[static_cast<size_t>(code)] & 0x80) && (m_prevKeyState[static_cast<size_t>(code)] & 0x80);
}

bool InputImpl::GetMouseButton(MOUSE_BUTTON button) const
{
	assert(static_cast<size_t>(button) < MOUSE_BUTTON::COUNT);
	return m_currMouseState.rgbButtons[static_cast<size_t>(button)] & 0x80;
}

bool InputImpl::GetMouseButtonDown(MOUSE_BUTTON button) const
{
	assert(static_cast<size_t>(button) < MOUSE_BUTTON::COUNT);
	return (m_currMouseState.rgbButtons[static_cast<size_t>(button)] & 0x80) && !(m_prevMouseBtnState[static_cast<size_t>(button)] & 0x80);
}

bool InputImpl::GetMouseButtonUp(MOUSE_BUTTON button) const
{
	assert(static_cast<size_t>(button) < MOUSE_BUTTON::COUNT);
	return !(m_currMouseState.rgbButtons[static_cast<size_t>(button)] & 0x80) && (m_prevMouseBtnState[static_cast<size_t>(button)] & 0x80);
}
