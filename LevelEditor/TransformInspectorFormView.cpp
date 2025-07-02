// InspectorFormView.cpp : implementation file
//

#include "TransformInspectorFormView.h"

// CTransformInspectorFormView

IMPLEMENT_DYNCREATE(CTransformInspectorFormView, CFormView)

CTransformInspectorFormView::CTransformInspectorFormView()
	: CFormView(IDD_TRANSFORM_INSPECTOR_FORMVIEW)
	, m_positionX()
	, m_positionY()
	, m_positionZ()
	, m_rotationX()
	, m_rotationY()
	, m_rotationZ()
	, m_scaleX()
	, m_scaleY()
	, m_scaleZ()
{
}

CTransformInspectorFormView::~CTransformInspectorFormView()
{
}

void CTransformInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_POSITIONX, m_positionX);
	DDX_Control(pDX, IDC_EDIT_POSITIONY, m_positionY);
	DDX_Control(pDX, IDC_EDIT_POSITIONZ, m_positionZ);
	DDX_Control(pDX, IDC_EDIT_ROTATIONX, m_rotationX);
	DDX_Control(pDX, IDC_EDIT_ROTATIONY, m_rotationY);
	DDX_Control(pDX, IDC_EDIT_ROTATIONZ, m_rotationZ);
	DDX_Control(pDX, IDC_EDIT_SCALEX, m_scaleX);
	DDX_Control(pDX, IDC_EDIT_SCALEY, m_scaleY);
	DDX_Control(pDX, IDC_EDIT_SCALEZ, m_scaleZ);
}

BEGIN_MESSAGE_MAP(CTransformInspectorFormView, CFormView)
END_MESSAGE_MAP()


// CTransformInspectorFormView diagnostics

#ifdef _DEBUG
void CTransformInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTransformInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG
