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
	float GetValue() const;	// Window Text -> Float ��ȯ

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
