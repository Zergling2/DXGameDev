
// LevelEditorView.h : interface of the CLevelEditorView class
//

#pragma once

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
protected:

// Implementation
public:
	virtual ~CLevelEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LevelEditorView.cpp
inline CLevelEditorDoc* CLevelEditorView::GetDocument() const
{
	return reinterpret_cast<CLevelEditorDoc*>(m_pDocument);
}
#endif

