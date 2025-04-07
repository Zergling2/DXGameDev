#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>

namespace ze
{
	enum class Axis
	{
		HORIZONTAL,
		VERTICAL
	};

	class InputImpl : public ISubsystem
	{
		friend class RuntimeImpl;
		ZE_DECLARE_SINGLETON(InputImpl);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		void Update();
		void FinalUpdate();
	public:
		bool GetKey(uint8_t keyCode);
		bool GetKeyDown(uint8_t keyCode);
		bool GetKeyUp(uint8_t keyCode);
		inline int32_t GetMouseMoveX() const { return static_cast<int32_t>(m_mouseState.lX); }
		inline int32_t GetMouseMoveY() const { return static_cast<int32_t>(m_mouseState.lY); }
		inline POINT GetMousePosition() { return m_mousePos; }
	private:
		ComPtr<IDirectInput8> m_cpDirectInput;
		ComPtr<IDirectInputDevice8> m_cpDIKeyboard;
		ComPtr<IDirectInputDevice8> m_cpDIMouse;
		alignas(64) uint8_t m_prevKeyState[256];
		alignas(64) uint8_t m_currKeyState[256];
		DIMOUSESTATE m_mouseState;
		POINT m_mousePos;
	};

	extern InputImpl Input;
}
