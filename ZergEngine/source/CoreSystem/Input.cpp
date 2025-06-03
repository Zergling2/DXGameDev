#include <ZergEngine\CoreSystem\Input.h>
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\Window.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

namespace ze
{
	InputImpl Input;
}

using namespace ze;

static PCWSTR UPDATE_LOG_PREFIX = L"InputImpl::Update > ";

InputImpl::InputImpl()
	: m_mode(INPUT_MODE::GAMEPLAY)
	, m_cpDirectInput(nullptr)
	, m_cpDIKeyboard(nullptr)
	, m_cpDIMouse(nullptr)
	, m_prevKeyState{}
	, m_currKeyState{}
	, m_prevMouseBtnState{}
	, m_currMouseState{}
	, m_mousePosition{ 0, 0 }
{
	XMStoreFloat3A(&m_mousePositionFlt, XMVectorZero());
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
	m_mousePosition.x = 0;
	m_mousePosition.y = 0;

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
	// 마우스 커서 위치 업데이트
	{
		POINT mousePosition;
		GetCursorPos(&mousePosition);

		if (!Window.IsFullscreen())
		{
			ScreenToClient(Window.GetWindowHandle(), &mousePosition);
			mousePosition.y = Window.GetHeightInteger() - mousePosition.y;	// y는 유니티 스크린 좌표 시스템과 동일하게 상하 반전
		}
		else
		{
			mousePosition.y = GetSystemMetrics(SM_CYSCREEN) - mousePosition.y;
		}

		m_mousePosition = mousePosition;
		m_mousePositionFlt = XMFLOAT3A(static_cast<FLOAT>(m_mousePosition.x), static_cast<FLOAT>(m_mousePosition.y), 0.0f);
	}

	HRESULT hr;
	memcpy(m_prevKeyState, m_currKeyState, sizeof(m_prevKeyState));
	memcpy(m_prevMouseBtnState, m_currMouseState.rgbButtons, sizeof(m_prevMouseBtnState));

	switch (m_mode)
	{
	case INPUT_MODE::GAMEPLAY:
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
		break;
	case INPUT_MODE::UI_INPUT_WAITING:
		// DirectInput의 입력 차단
		ZeroMemory(m_currKeyState, sizeof(m_currKeyState));
		ZeroMemory(&m_currMouseState, sizeof(m_currMouseState));
		break;
	}
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
	assert(static_cast<size_t>(button) < static_cast<size_t>(MOUSE_BUTTON::COUNT));
	return m_currMouseState.rgbButtons[static_cast<size_t>(button)] & 0x80;
}

bool InputImpl::GetMouseButtonDown(MOUSE_BUTTON button) const
{
	assert(static_cast<size_t>(button) < static_cast<size_t>(MOUSE_BUTTON::COUNT));
	return (m_currMouseState.rgbButtons[static_cast<size_t>(button)] & 0x80) && !(m_prevMouseBtnState[static_cast<size_t>(button)] & 0x80);
}

bool InputImpl::GetMouseButtonUp(MOUSE_BUTTON button) const
{
	assert(static_cast<size_t>(button) < static_cast<size_t>(MOUSE_BUTTON::COUNT));
	return !(m_currMouseState.rgbButtons[static_cast<size_t>(button)] & 0x80) && (m_prevMouseBtnState[static_cast<size_t>(button)] & 0x80);
}
