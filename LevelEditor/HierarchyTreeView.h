#pragma once

#include "framework.h"

// CHierarchyTreeView view

class CHierarchyTreeView : public CTreeView
{
	DECLARE_DYNCREATE(CHierarchyTreeView)

protected:
	CHierarchyTreeView();           // protected constructor used by dynamic creation
	virtual ~CHierarchyTreeView();

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
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


