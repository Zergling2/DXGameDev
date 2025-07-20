#pragma once

#include "..\framework.h"

// CFloatEdit

class CFloatEdit : public CEdit
{
	DECLARE_DYNAMIC(CFloatEdit)
public:
	CFloatEdit();
	virtual ~CFloatEdit();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
