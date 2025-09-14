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

	// ������: �����찡 ��Ŀ���� ���� ������ ��쿡�� Cursor ���� ���� �Լ��� �÷��׸� ������ �� �ƹ��͵� ���� �ʰ� �����ϸ� ��.
	// �� �ڿ� �����찡 �ٽ� ��Ŀ���� ��� ��쿡�� ��Ÿ���� Cursor�� ���� �÷��׸� ������ �״�� SetVisible, SetLockState �Լ����� ȣ�����ָ� ��.



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
