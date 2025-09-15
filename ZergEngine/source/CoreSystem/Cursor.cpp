#include <ZergEngine\CoreSystem\Cursor.h>
#include <ZergEngine\CoreSystem\Runtime.h>

using namespace ze;

bool Cursor::s_changeOnlyFlag = false;
Cursor::Context Cursor::s_context =
{
	true,
	CursorLockMode::None
};

void Cursor::SetVisible(bool visible)
{
	if (s_changeOnlyFlag)
	{
		s_context.m_visible = visible;
		return;
	}

	CURSORINFO ci;
	ci.cbSize = sizeof(ci);

	GetCursorInfo(&ci);

	if (visible)
	{
		if (ci.flags == 0)
			while (ShowCursor(TRUE) < 0);	// 0 이상이 될 때까지 반복 호출
	}
	else
	{
		if (ci.flags == CURSOR_SHOWING)
			while (ShowCursor(FALSE) >= 0);	// 0 미만이 될 때까지 반복 호출
	}

	s_context.m_visible = visible;
}

void Cursor::SetLockState(CursorLockMode mode)
{
	if (s_changeOnlyFlag)
	{
		s_context.m_lockMode = mode;
		return;
	}

	switch (mode)
	{
	case CursorLockMode::None:
		Cursor::UnlockCursor();
		break;
	case CursorLockMode::Locked:
		Cursor::LockCursor();
		break;
	case CursorLockMode::Confined:
		Cursor::ConfineCursor();
		break;
	}

	s_context.m_lockMode = mode;
}

void Cursor::UnlockCursor()
{
	ClipCursor(nullptr);
}

void Cursor::LockCursor()
{
	HWND hGameWnd = Runtime::GetInstance()->GetGameWindowHandle();

	RECT cr;	// Client rect
	GetClientRect(hGameWnd, &cr);

	POINT center;
	center.x = (cr.right - cr.left) / 2;
	center.y = (cr.bottom - cr.top) / 2;
	ClientToScreen(hGameWnd, &center);

	RECT clipRect;
	clipRect.left = center.x;
	clipRect.right = center.x + 1;
	clipRect.top = center.y;
	clipRect.bottom = center.y + 1;

	ClipCursor(&clipRect);
}

void Cursor::ConfineCursor()
{
	HWND hGameWnd = Runtime::GetInstance()->GetGameWindowHandle();

	RECT cr;	// Client rect
	GetClientRect(hGameWnd, &cr);

	POINT lt;	// Left Top
	POINT rb;	// Right Bottom
	lt.x = cr.left;
	lt.y = cr.top;
	rb.x = cr.right;
	rb.y = cr.bottom;

	ClientToScreen(hGameWnd, &lt);
	ClientToScreen(hGameWnd, &rb);

	RECT clipRect;
	clipRect.left = lt.x;
	clipRect.top = lt.y;
	clipRect.right = rb.x;
	clipRect.bottom = rb.y;

	ClipCursor(&clipRect);
}
