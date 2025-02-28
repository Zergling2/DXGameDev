#pragma once

#include "framework.h"

// CLogListView view

class CLogListView : public CListView
{
	DECLARE_DYNCREATE(CLogListView)

protected:
	CLogListView();           // protected constructor used by dynamic creation
	virtual ~CLogListView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
};


