#include <ZergEngine\CoreSystem\Cursor.h>
#define NOMINMAX
#include <Windows.h>
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
			while (ShowCursor(TRUE) >= 0);	// 0 미만이 될 때까지 반복 호출
	}
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

	POINT tl = { cr.left, cr.top };
	POINT br = { cr.right, cr.bottom };

	ClientToScreen(hGameWnd, &tl);
	ClientToScreen(hGameWnd, &br);
	RECT clipRect = { tl.x, tl.y, br.x, br.y };

	ClipCursor(&clipRect);
}

void Cursor::ConfineCursor()
{
	HWND hGameWnd = Runtime::GetInstance()->GetGameWindowHandle();

	RECT cr;	// Client rect
	GetClientRect(hGameWnd, &cr);

	POINT tl = { cr.left, cr.top };
	POINT br = { cr.right, cr.bottom };

	ClientToScreen(hGameWnd, &tl);
	ClientToScreen(hGameWnd, &br);
	RECT clipRect = { tl.x, tl.y, br.x, br.y };

	ClipCursor(&clipRect);
}
