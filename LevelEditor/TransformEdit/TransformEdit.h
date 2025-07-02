#pragma once

#include "..\framework.h"

// CTransformEdit

class CTransformEdit : public CEdit
{
	DECLARE_DYNAMIC(CTransformEdit)
public:
	CTransformEdit();
	virtual ~CTransformEdit();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
