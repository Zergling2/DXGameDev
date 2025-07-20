#include "TerrainInspectorFormView.h"

// CTerrainInspectorFormView

IMPLEMENT_DYNCREATE(CTerrainInspectorFormView, CFormView)

CTerrainInspectorFormView::CTerrainInspectorFormView()
	: CFormView(IDD_TERRAIN_INSPECTOR_FORMVIEW)
	, m_pItem(nullptr)
{
}

CTerrainInspectorFormView::~CTerrainInspectorFormView()
{
}

void CTerrainInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_BRUSH_TERRAIN, m_radioBrushTerrain);
	DDX_Control(pDX, IDC_COMBO_ELEVATION_MODIFICATION, m_elevationModification);
	DDX_Control(pDX, IDC_STATIC_TERRAIN_BRUSH_SIZE, m_staticTerrainBrushSize);
	DDX_Control(pDX, IDC_STATIC_TERRAIN_BRUSH_OPACITY, m_staticTerrainBrushOpacity);
	DDX_Control(pDX, IDC_SLIDER_TERRAIN_BRUSH_SIZE, m_sliderTerrainBrushSize);
	DDX_Control(pDX, IDC_SLIDER_TERRAIN_BRUSH_OPACITY, m_sliderTerrainBrushOpacity);
	DDX_Control(pDX, IDC_EDIT_TERRAIN_BRUSH_SIZE, m_editTerrainBrushSize);
	DDX_Control(pDX, IDC_EDIT_TERRAIN_BRUSH_OPACITY, m_editTerrainBrushOpacity);
	DDX_Control(pDX, IDC_STATIC_TERRAIN_LAYERS, m_staticTerrainLayers);
	DDX_Control(pDX, IDC_TERRAIN_LAYER0_IMAGE, m_imgTerrainLayer0);
	DDX_Control(pDX, IDC_TERRAIN_LAYER1_IMAGE, m_imgTerrainLayer1);
	DDX_Control(pDX, IDC_TERRAIN_LAYER2_IMAGE, m_imgTerrainLayer2);
	DDX_Control(pDX, IDC_TERRAIN_LAYER3_IMAGE, m_imgTerrainLayer3);
	DDX_Control(pDX, IDC_TERRAIN_LAYER4_IMAGE, m_imgTerrainLayer4);
}

void CTerrainInspectorFormView::HideTerrainBrushUITools()
{
	m_elevationModification.ShowWindow(SW_HIDE);
	m_staticTerrainBrushSize.ShowWindow(SW_HIDE);
	m_staticTerrainBrushOpacity.ShowWindow(SW_HIDE);
	m_sliderTerrainBrushSize.ShowWindow(SW_HIDE);
	m_sliderTerrainBrushOpacity.ShowWindow(SW_HIDE);
	m_editTerrainBrushSize.ShowWindow(SW_HIDE);
	m_editTerrainBrushOpacity.ShowWindow(SW_HIDE);
	m_staticTerrainLayers.ShowWindow(SW_HIDE);
	m_imgTerrainLayer0.ShowWindow(SW_HIDE);
	m_imgTerrainLayer1.ShowWindow(SW_HIDE);
	m_imgTerrainLayer2.ShowWindow(SW_HIDE);
	m_imgTerrainLayer3.ShowWindow(SW_HIDE);
	m_imgTerrainLayer4.ShowWindow(SW_HIDE);
}

void CTerrainInspectorFormView::ShowTerrainBrushUITools()
{
	m_elevationModification.ShowWindow(SW_SHOW);
	m_staticTerrainBrushSize.ShowWindow(SW_SHOW);
	m_staticTerrainBrushOpacity.ShowWindow(SW_SHOW);
	m_sliderTerrainBrushSize.ShowWindow(SW_SHOW);
	m_sliderTerrainBrushOpacity.ShowWindow(SW_SHOW);
	m_editTerrainBrushSize.ShowWindow(SW_SHOW);
	m_editTerrainBrushOpacity.ShowWindow(SW_SHOW);
	m_staticTerrainLayers.ShowWindow(SW_SHOW);
	m_imgTerrainLayer0.ShowWindow(SW_SHOW);
	m_imgTerrainLayer1.ShowWindow(SW_SHOW);
	m_imgTerrainLayer2.ShowWindow(SW_SHOW);
	m_imgTerrainLayer3.ShowWindow(SW_SHOW);
	m_imgTerrainLayer4.ShowWindow(SW_SHOW);
}


BEGIN_MESSAGE_MAP(CTerrainInspectorFormView, CFormView)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_BRUSH_TERRAIN, IDC_RADIO_BRUSH_GRASS, &CTerrainInspectorFormView::OnBnClickedTerrainBrushType)
END_MESSAGE_MAP()


// CTerrainInspectorFormView diagnostics

#ifdef _DEBUG
void CTerrainInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTerrainInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTerrainInspectorFormView message handlers

void CTerrainInspectorFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_radioBrushTerrain.SetCheck(TRUE);
	m_currentTerrainBrushType = TERRAIN_BRUSH_TYPE::BRUSH_TERRAIN;

	// TODO: Add your specialized code here and/or call the base class
	m_elevationModification.AddString(_T("Raise or Lower Terrain"));
	m_elevationModification.AddString(_T("Paint Texture"));
	m_elevationModification.AddString(_T("Set Height"));
	m_elevationModification.AddString(_T("Smooth Height"));

	// Raise or Lower Terrain 기본 선택
	m_elevationModification.SetCurSel(0);
}

void CTerrainInspectorFormView::OnBnClickedTerrainBrushType(UINT id)
{
	switch (id)
	{
	case IDC_RADIO_BRUSH_TERRAIN:
		if (m_currentTerrainBrushType != TERRAIN_BRUSH_TYPE::BRUSH_TERRAIN)
		{
			this->ShowTerrainBrushUITools();
			m_currentTerrainBrushType = TERRAIN_BRUSH_TYPE::BRUSH_TERRAIN;
		}
		break;
	case IDC_RADIO_BRUSH_TREE:
		if (m_currentTerrainBrushType != TERRAIN_BRUSH_TYPE::BRUSH_TREE)
		{
			this->HideTerrainBrushUITools();
			m_currentTerrainBrushType = TERRAIN_BRUSH_TYPE::BRUSH_TREE;
		}
		break;
	case IDC_RADIO_BRUSH_GRASS:
		if (m_currentTerrainBrushType != TERRAIN_BRUSH_TYPE::BRUSH_GRASS)
		{
			this->HideTerrainBrushUITools();
			m_currentTerrainBrushType = TERRAIN_BRUSH_TYPE::BRUSH_GRASS;
		}
		m_currentTerrainBrushType = TERRAIN_BRUSH_TYPE::BRUSH_GRASS;
		break;
	default:
		assert(false);
		break;
	}
}
