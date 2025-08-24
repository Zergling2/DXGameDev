#pragma once

#include "..\framework.h"

// CRangeIntEdit

class CRangeIntEdit : public CEdit
{
	DECLARE_DYNAMIC(CRangeIntEdit)

public:
	CRangeIntEdit();
	virtual ~CRangeIntEdit();

protected:
	DECLARE_MESSAGE_MAP()
private:
	int min;
	int max;
};
