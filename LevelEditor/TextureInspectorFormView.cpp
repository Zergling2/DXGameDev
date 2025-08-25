#include "LevelEditor.h"
#include "TextureInspectorFormView.h"

// CTextureInspectorFormView

IMPLEMENT_DYNCREATE(CTextureInspectorFormView, CFormView)

CTextureInspectorFormView::CTextureInspectorFormView()
	: CFormView(IDD_TEXTURE_INSPECTOR_FORMVIEW)
	, m_staticTextureArraySize(0)
	, m_staticTextureMipLevels(0)
	, m_staticTextureType(_T(""))
	, m_staticTextureFormat(_T(""))
	, m_staticTextureSize(_T(""))
{
}

CTextureInspectorFormView::~CTextureInspectorFormView()
{
}

void CTextureInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_TEXTURE_ARRAY_SIZE, m_staticTextureArraySize);
	DDX_Text(pDX, IDC_STATIC_TEXTURE_MIP_LEVELS, m_staticTextureMipLevels);
	DDX_Text(pDX, IDC_STATIC_TEXTURE_TYPE, m_staticTextureType);
	DDX_Text(pDX, IDC_STATIC_TEXTURE_FORMAT, m_staticTextureFormat);
	DDX_Text(pDX, IDC_STATIC_TEXTURE_SIZE, m_staticTextureSize);
}

BEGIN_MESSAGE_MAP(CTextureInspectorFormView, CFormView)
END_MESSAGE_MAP()


// CTextureInspectorFormView diagnostics

#ifdef _DEBUG
void CTextureInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTextureInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTextureInspectorFormView message handlers
