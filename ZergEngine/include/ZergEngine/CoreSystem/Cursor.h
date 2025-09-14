#pragma once

namespace ze
{
	// How the cursor should behave.
	enum class CursorLockMode
	{
		None,		// Cursor behavior is unmodified.
		Locked,		// Locks the cursor to the center of the game view.
		Confined	// Confine cursor to the game window.
	};

	// 설계방안: 윈도우가 포커스를 잃은 상태일 경우에는 Cursor 상태 변경 함수가 플래그만 변경한 뒤 아무것도 하지 않고 리턴하면 됨.
	// 그 뒤에 윈도우가 다시 포커스를 얻는 경우에는 런타임이 Cursor의 상태 플래그를 가지고 그대로 SetVisible, SetLockState 함수들을 호출해주면 됨.



	class Cursor
	{
		friend class Runtime;
	public:
		struct Context
		{
			bool m_visible;
			CursorLockMode m_lockMode;
		};
	public:
		Cursor() = delete;

		static void SetVisible(bool visible);
		static void SetLockState(CursorLockMode mode);

		static bool IsVisible() { return s_context.m_visible; }
		static CursorLockMode GetLockState() { return s_context.m_lockMode; }
		static Context GetContext() { return s_context; }
	private:
		static void SetChangeOnlyFlag(bool flag) { s_changeOnlyFlag = flag; }

		static void UnlockCursor();
		static void LockCursor();
		static void ConfineCursor();
	private:
		static bool s_changeOnlyFlag;
		static Context s_context;
	};
}
