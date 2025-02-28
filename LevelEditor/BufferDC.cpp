#include "BufferDC.h"

CBufferDC::CBufferDC(CWnd* pParent)
	: m_pParent(pParent)
{
	ASSERT(pParent);

	// Get target window's client rect info.
	m_pTarget = m_pParent->BeginPaint(&m_ps);
	m_pParent->GetClientRect(&m_clientRect);
	m_pParent->GetWindowRect(&m_windowRect);

	// 대상 윈도우에 대한 DC를 생성한다.
	m_memDC.CreateCompatibleDC(m_pTarget);

	// 대상 DC에 대한 메모리 비트맵을 생성하여 Select 한다.
	m_memBmp.CreateBitmap(m_clientRect.Width(), m_clientRect.Height(),
		m_memDC.GetDeviceCaps(PLANES),
		m_memDC.GetDeviceCaps(BITSPIXEL), 0);
	m_pOldMemBmp = m_memDC.SelectObject(&m_memBmp);

	// 메모리 버퍼에 Attach한다.
	Attach(m_memDC);
}


CBufferDC::~CBufferDC()
{
	// 메모리 DC의 내용을 대상 윈도우에 출력한다.
	// 내부적으로 비트맵에 출력한 것이므로 해당 비트맵을 1:1로 복사한다.
	m_pTarget->BitBlt(
		m_ps.rcPaint.left,
		m_ps.rcPaint.top,
		m_ps.rcPaint.right - m_ps.rcPaint.left,
		m_ps.rcPaint.bottom - m_ps.rcPaint.top,
		&m_memDC,
		m_ps.rcPaint.left,
		m_ps.rcPaint.top, SRCCOPY);

	m_memDC.SelectObject(m_pOldMemBmp);
	m_pParent->EndPaint(&m_ps);

	Detach();
}


BOOL CBufferDC::Attach(HDC hDC)
{
	return CDC::Attach(hDC);
}


HDC CBufferDC::Detach()
{
	return CDC::Detach();
}