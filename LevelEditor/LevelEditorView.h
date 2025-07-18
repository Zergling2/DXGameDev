#pragma once

#include "framework.h"

class CLevelEditorDoc;

class CLevelEditorView : public CView
{
protected: // create from serialization only
	CLevelEditorView() noexcept;
	DECLARE_DYNCREATE(CLevelEditorView)
// Attributes
public:
	CLevelEditorDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
// Implementation
public:
	virtual ~CLevelEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in LevelEditorView.cpp
inline CLevelEditorDoc* CLevelEditorView::GetDocument() const
{
	return reinterpret_cast<CLevelEditorDoc*>(m_pDocument);
}
#endif
