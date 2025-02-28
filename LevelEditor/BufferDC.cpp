#include "BufferDC.h"

CBufferDC::CBufferDC(CWnd* pParent)
	: m_pParent(pParent)
{
	ASSERT(pParent);

	// Get target window's client rect info.
	m_pTarget = m_pParent->BeginPaint(&m_ps);
	m_pParent->GetClientRect(&m_clientRect);
	m_pParent->GetWindowRect(&m_windowRect);

	// ��� �����쿡 ���� DC�� �����Ѵ�.
	m_memDC.CreateCompatibleDC(m_pTarget);

	// ��� DC�� ���� �޸� ��Ʈ���� �����Ͽ� Select �Ѵ�.
	m_memBmp.CreateBitmap(m_clientRect.Width(), m_clientRect.Height(),
		m_memDC.GetDeviceCaps(PLANES),
		m_memDC.GetDeviceCaps(BITSPIXEL), 0);
	m_pOldMemBmp = m_memDC.SelectObject(&m_memBmp);

	// �޸� ���ۿ� Attach�Ѵ�.
	Attach(m_memDC);
}


CBufferDC::~CBufferDC()
{
	// �޸� DC�� ������ ��� �����쿡 ����Ѵ�.
	// ���������� ��Ʈ�ʿ� ����� ���̹Ƿ� �ش� ��Ʈ���� 1:1�� �����Ѵ�.
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