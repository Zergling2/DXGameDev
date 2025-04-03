#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	enum class Axis
	{
		HORIZONTAL,
		VERTICAL
	};

	class Input : public ISubsystem
	{
		friend class Runtime;
		ZE_DECLARE_SINGLETON(Input);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void Update();
	public:
		inline int32_t GetMouseMoveX() const { return static_cast<int32_t>(m_mouseState.lX); }
		inline int32_t GetMouseMoveY() const { return static_cast<int32_t>(m_mouseState.lY); }
		inline POINT GetMousePosition() { return m_mousePos; }
	private:
		ComPtr<IDirectInput8> m_cpDirectInput;
		ComPtr<IDirectInputDevice8> m_cpDIKeyboard;
		ComPtr<IDirectInputDevice8> m_cpDIMouse;
		uint8_t m_keyboardState[256];
		DIMOUSESTATE m_mouseState;
		POINT m_mousePos;
	};
}
