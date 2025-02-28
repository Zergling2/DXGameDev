#pragma once

#include "framework.h"

class CBufferDC : public CDC
{
public:
	CBufferDC(CWnd* pParent);
	CBufferDC() = delete;
	CBufferDC(const CBufferDC& src) = delete;
	CBufferDC& operator=(const CBufferDC& src) = delete;
	~CBufferDC();
protected:
	BOOL Attach(HDC hDC);
	HDC Detach();
private:
	CWnd* m_pParent;
	CDC* m_pTarget;
	PAINTSTRUCT m_ps;

	// Target size info
	CRect m_clientRect;
	CRect m_windowRect;

	CDC m_memDC;						// Buffer DC
	CBitmap m_memBmp;
	CBitmap* m_pOldMemBmp;				// for buffering
public:
	inline CRect ClientRect() const { return m_clientRect; }
	inline CRect WindowRect() const { return m_windowRect; }
	inline CRect UpdateRect() const { return m_ps.rcPaint; }

	operator HDC() const { return m_memDC.m_hDC; }       //  DC handle for API functions
};
