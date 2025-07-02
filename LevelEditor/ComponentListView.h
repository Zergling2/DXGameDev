#pragma once

#include "framework.h"

// CComponentListView view

class CComponentListView : public CListView
{
	DECLARE_DYNCREATE(CComponentListView)
protected:
	CComponentListView();           // protected constructor used by dynamic creation
	virtual ~CComponentListView();
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
protected:
	DECLARE_MESSAGE_MAP()
private:
	bool m_initialized;
public:
	virtual void OnInitialUpdate();
};


