#include <ZergEngine\CoreSystem\Input.h>
#include <ZergEngine\CoreSystem\Debug.h>

using namespace ze;

Input* Input::s_pInstance = nullptr;

Input::Input()
	: m_mode(INPUT_MODE::GAMEPLAY)
	, m_cpDirectInput(nullptr)
	, m_cpDIKeyboard(nullptr)
	, m_cpDIMouse(nullptr)
	, m_prevKeyState{}
	, m_currKeyState{}
	, m_prevMouseBtnState{}
	, m_currMouseState{}
	, m_mousePositionFlt(0.0f, 0.0f, 0.0f)
	, m_mousePosition{ 0, 0 }
{
}

Input::~Input()
{
}

void Input::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new Input();
}

void Input::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void Input::Init(HINSTANCE hInstance, HWND hWnd)
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
		hInstance,
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

	hr = m_cpDIKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);	// DISCL_EXCLUSIVE
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > m_cpDIKeyboard->SetCooperativeLevel() failed.", hr);

	hr = m_cpDIKeyboard->Acquire();

	// Mouse 인터페이스 생성
	hr = m_cpDirectInput->CreateDevice(GUID_SysMouse, m_cpDIMouse.GetAddressOf(), nullptr);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > m_cpDirectInput->CreateDevice() failed.", hr);

	hr = m_cpDIMouse->SetDataFormat(&c_dfDIMouse2);
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > m_cpDIMouse->SetDataFormat() failed.", hr);

	hr = m_cpDIMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);	// DISCL_NONEXCLUSIVE
	if (FAILED(hr))
		Debug::ForceCrashWithHRESULTErrorMessageBox(L"Input::Init() > m_cpDIMouse->SetCooperativeLevel() failed.", hr);

	hr = m_cpDIMouse->Acquire();
}

void Input::UnInit()
{
	if (m_cpDIMouse)
		m_cpDIMouse->Unacquire();

	if (m_cpDIKeyboard)
		m_cpDIKeyboard->Unacquire();

	m_cpDIMouse.Reset();
	m_cpDIKeyboard.Reset();
	m_cpDirectInput.Reset();
}

void Input::SetMousePos(POINT pt)
{
	m_mousePosition = pt;
	m_mousePositionFlt = XMFLOAT3A(static_cast<FLOAT>(m_mousePosition.x), static_cast<FLOAT>(m_mousePosition.y), 0.0f);
}

void Input::Update()
{
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

bool Input::GetKey(KEYCODE code) const
{
	assert(static_cast<size_t>(code) < 256);
	return m_currKeyState[static_cast<size_t>(code)] & 0x80;
}

bool Input::GetKeyDown(KEYCODE code) const
{
	assert(static_cast<size_t>(code) < 256);
	return (m_currKeyState[static_cast<size_t>(code)] & 0x80) && !(m_prevKeyState[static_cast<size_t>(code)] & 0x80);
}

bool Input::GetKeyUp(KEYCODE code) const
{
	assert(static_cast<size_t>(code) < 256);
	return !(m_currKeyState[static_cast<size_t>(code)] & 0x80) && (m_prevKeyState[static_cast<size_t>(code)] & 0x80);
}

bool Input::GetMouseButton(MOUSE_BUTTON button) const
{
	assert(static_cast<size_t>(button) < static_cast<size_t>(MOUSE_BUTTON::COUNT));
	return m_currMouseState.rgbButtons[static_cast<size_t>(button)] & 0x80;
}

bool Input::GetMouseButtonDown(MOUSE_BUTTON button) const
{
	assert(static_cast<size_t>(button) < static_cast<size_t>(MOUSE_BUTTON::COUNT));
	return (m_currMouseState.rgbButtons[static_cast<size_t>(button)] & 0x80) && !(m_prevMouseBtnState[static_cast<size_t>(button)] & 0x80);
}

bool Input::GetMouseButtonUp(MOUSE_BUTTON button) const
{
	assert(static_cast<size_t>(button) < static_cast<size_t>(MOUSE_BUTTON::COUNT));
	return !(m_currMouseState.rgbButtons[static_cast<size_t>(button)] & 0x80) && (m_prevMouseBtnState[static_cast<size_t>(button)] & 0x80);
}
