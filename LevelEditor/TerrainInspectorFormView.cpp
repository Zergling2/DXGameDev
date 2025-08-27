#include "TerrainInspectorFormView.h"
#include "LevelEditor.h"
#include "AssetTreeView.h"
#include "MainFrm.h"
#include "CLVItem\CLVItemTerrain.h"
#include "ATVItem\ATVItemTexture.h"

// CTerrainInspectorFormView

IMPLEMENT_DYNCREATE(CTerrainInspectorFormView, CFormView)

CTerrainInspectorFormView::CTerrainInspectorFormView()
	: CFormView(IDD_TERRAIN_INSPECTOR_FORMVIEW)
	, m_tde()
	, m_pItem(nullptr)
	, m_currentTerrainBrushType(TERRAIN_BRUSH_TYPE::BRUSH_TERRAIN)
	, m_currentTerrainEditMode(TERRAIN_EDIT_MODE::RAISE_OR_LOWER_TERRAIN)
{
}

CTerrainInspectorFormView::~CTerrainInspectorFormView()
{
}

void CTerrainInspectorFormView::SetCLVItemToModify(CLVItemTerrain* pItem)
{
	m_tde.SetTerrainToModify(pItem->GetTerrain());
	m_pItem = pItem;
}

void CTerrainInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RADIO_BRUSH_TERRAIN, m_radioBrushTerrain);
	DDX_Control(pDX, IDC_COMBO_TERRAIN_EDIT_MODE, m_comboTerrainEditMode);
	DDX_Control(pDX, IDC_STATIC_TERRAIN_BRUSH_SIZE, m_staticTerrainBrushSize);
	DDX_Control(pDX, IDC_STATIC_TERRAIN_BRUSH_OPACITY, m_staticTerrainBrushOpacity);
	DDX_Control(pDX, IDC_SLIDER_TERRAIN_BRUSH_SIZE, m_sliderTerrainBrushSize);
	DDX_Control(pDX, IDC_SLIDER_TERRAIN_BRUSH_OPACITY, m_sliderTerrainBrushOpacity);
	DDX_Control(pDX, IDC_EDIT_TERRAIN_BRUSH_SIZE, m_editTerrainBrushSize);
	DDX_Control(pDX, IDC_EDIT_TERRAIN_BRUSH_OPACITY, m_editTerrainBrushOpacity);
	DDX_Control(pDX, IDC_STATIC_TERRAIN_LAYERS, m_staticTerrainLayers);
	DDX_Control(pDX, IDC_COMBO_TERRAIN_DIFFUSE_LAYER, m_comboTerrainDiffuseLayer);
	DDX_Control(pDX, IDC_COMBO_TERRAIN_NORMAL_LAYER, m_comboTerrainNormalLayer);
	DDX_Control(pDX, IDC_STATIC_LAYER_MODIFYING, m_staticLayerModifying);
	DDX_Control(pDX, IDC_RADIO_LAYER0, m_radioLayerModifying0);
	DDX_Control(pDX, IDC_RADIO_LAYER1, m_radioLayerModifying1);
	DDX_Control(pDX, IDC_RADIO_LAYER2, m_radioLayerModifying2);
	DDX_Control(pDX, IDC_RADIO_LAYER3, m_radioLayerModifying3);
	DDX_Control(pDX, IDC_RADIO_LAYER4, m_radioLayerModifying4);
	DDX_Control(pDX, IDC_STATIC_TERRAIN_DIFFUSE_LAYER, m_staticTerrainDiffuseLayer);
	DDX_Control(pDX, IDC_STATIC_TERRAIN_NORMAL_LAYER, m_staticTerrainNormalLayer);
	DDX_Control(pDX, IDC_BUTTON_REMOVE_TERRAIN_DIFFUSE_LAYER, m_buttonRemoveTerrainDiffuseLayer);
	DDX_Control(pDX, IDC_BUTTON_REMOVE_TERRAIN_NORMAL_LAYER, m_buttonRemoveTerrainNormalLayer);
	DDX_Control(pDX, IDC_STATIC_SET_TERRAIN_HEIGHT_VALUE, m_staticSetTerrainHeightValue);
	DDX_Control(pDX, IDC_EDIT_SET_TERRAIN_HEIGHT_VALUE, m_editSetTerrainHeightValue);
	DDX_Control(pDX, IDC_STATIC_SMOOTH_TERRAIN_HEIGHT_BLUR_DIRECTION, m_staticSmoothTerrainHeightBlurDirection);
	DDX_Control(pDX, IDC_SLIDER_SMOOTH_TERRAIN_HEIGHT_BLUR_DIRECTION, m_sliderSmoothTerrainHeightBlurDirection);
	DDX_Control(pDX, IDC_EDIT_SMOOTH_TERRAIN_HEIGHT_BLUR_DIRECTION, m_editSmoothTerrainHeightBlurDirection);
	DDX_Control(pDX, IDC_STATIC_SET_TERRAIN_HEIGHT_VALUE_HELP, m_staticSetTerrainHeightValueHelp);
	DDX_Control(pDX, IDC_STATIC_TERRAIN_INFO, m_staticTerrainInfo);
}

void CTerrainInspectorFormView::OnHideTerrainBrushUI()
{
	// 공통
	m_comboTerrainEditMode.ShowWindow(SW_HIDE);
	m_staticTerrainBrushSize.ShowWindow(SW_HIDE);
	m_staticTerrainBrushOpacity.ShowWindow(SW_HIDE);
	m_sliderTerrainBrushSize.ShowWindow(SW_HIDE);
	m_sliderTerrainBrushOpacity.ShowWindow(SW_HIDE);
	m_editTerrainBrushSize.ShowWindow(SW_HIDE);
	m_editTerrainBrushOpacity.ShowWindow(SW_HIDE);
	// Raise or Lower Terrain

	// Paint Texture
	m_staticTerrainLayers.ShowWindow(SW_HIDE);
	m_staticTerrainDiffuseLayer.ShowWindow(SW_HIDE);
	m_staticTerrainNormalLayer.ShowWindow(SW_HIDE);
	m_comboTerrainDiffuseLayer.ShowWindow(SW_HIDE);
	m_comboTerrainNormalLayer.ShowWindow(SW_HIDE);
	m_buttonRemoveTerrainDiffuseLayer.ShowWindow(SW_HIDE);
	m_buttonRemoveTerrainNormalLayer.ShowWindow(SW_HIDE);
	m_staticLayerModifying.ShowWindow(SW_HIDE);
	m_radioLayerModifying0.ShowWindow(SW_HIDE);
	m_radioLayerModifying1.ShowWindow(SW_HIDE);
	m_radioLayerModifying2.ShowWindow(SW_HIDE);
	m_radioLayerModifying3.ShowWindow(SW_HIDE);
	m_radioLayerModifying4.ShowWindow(SW_HIDE);
	// Set Height
	m_staticSetTerrainHeightValue.ShowWindow(SW_HIDE);
	m_editSetTerrainHeightValue.ShowWindow(SW_HIDE);
	m_staticSetTerrainHeightValueHelp.ShowWindow(SW_HIDE);
	// Smooth Height
	m_staticSmoothTerrainHeightBlurDirection.ShowWindow(SW_HIDE);
	m_sliderSmoothTerrainHeightBlurDirection.ShowWindow(SW_HIDE);
	m_editSmoothTerrainHeightBlurDirection.ShowWindow(SW_HIDE);
}

void CTerrainInspectorFormView::OnShowTerrainBrushUI()
{
	// 공통 사항 컨트롤들 SHOW
	m_comboTerrainEditMode.ShowWindow(SW_SHOW);
	m_staticTerrainBrushSize.ShowWindow(SW_SHOW);
	m_staticTerrainBrushOpacity.ShowWindow(SW_SHOW);
	m_sliderTerrainBrushSize.ShowWindow(SW_SHOW);
	m_sliderTerrainBrushOpacity.ShowWindow(SW_SHOW);
	m_editTerrainBrushSize.ShowWindow(SW_SHOW);
	m_editTerrainBrushOpacity.ShowWindow(SW_SHOW);

	// 비 공통 컨트롤들은 일단 모두 HIDE
	// Paint Texture
	m_staticTerrainLayers.ShowWindow(SW_HIDE);
	m_staticTerrainDiffuseLayer.ShowWindow(SW_HIDE);
	m_staticTerrainNormalLayer.ShowWindow(SW_HIDE);
	m_comboTerrainDiffuseLayer.ShowWindow(SW_HIDE);
	m_comboTerrainNormalLayer.ShowWindow(SW_HIDE);
	m_buttonRemoveTerrainDiffuseLayer.ShowWindow(SW_HIDE);
	m_buttonRemoveTerrainNormalLayer.ShowWindow(SW_HIDE);
	m_staticLayerModifying.ShowWindow(SW_HIDE);
	m_radioLayerModifying0.ShowWindow(SW_HIDE);
	m_radioLayerModifying1.ShowWindow(SW_HIDE);
	m_radioLayerModifying2.ShowWindow(SW_HIDE);
	m_radioLayerModifying3.ShowWindow(SW_HIDE);
	m_radioLayerModifying4.ShowWindow(SW_HIDE);
	// Set Height
	m_staticSetTerrainHeightValue.ShowWindow(SW_HIDE);
	m_editSetTerrainHeightValue.ShowWindow(SW_HIDE);
	m_staticSetTerrainHeightValueHelp.ShowWindow(SW_HIDE);
	// Smooth Height
	m_staticSmoothTerrainHeightBlurDirection.ShowWindow(SW_HIDE);
	m_sliderSmoothTerrainHeightBlurDirection.ShowWindow(SW_HIDE);
	m_editSmoothTerrainHeightBlurDirection.ShowWindow(SW_HIDE);

	switch (m_currentTerrainEditMode)
	{
	case TERRAIN_EDIT_MODE::RAISE_OR_LOWER_TERRAIN:
		break;
	case TERRAIN_EDIT_MODE::PAINT_TEXTURE:
		m_staticTerrainLayers.ShowWindow(SW_SHOW);
		m_staticTerrainDiffuseLayer.ShowWindow(SW_SHOW);
		m_staticTerrainNormalLayer.ShowWindow(SW_SHOW);
		m_comboTerrainDiffuseLayer.ShowWindow(SW_SHOW);
		m_comboTerrainNormalLayer.ShowWindow(SW_SHOW);
		m_buttonRemoveTerrainDiffuseLayer.ShowWindow(SW_SHOW);
		m_buttonRemoveTerrainNormalLayer.ShowWindow(SW_SHOW);
		m_staticLayerModifying.ShowWindow(SW_SHOW);
		m_radioLayerModifying0.ShowWindow(SW_SHOW);
		m_radioLayerModifying1.ShowWindow(SW_SHOW);
		m_radioLayerModifying2.ShowWindow(SW_SHOW);
		m_radioLayerModifying3.ShowWindow(SW_SHOW);
		m_radioLayerModifying4.ShowWindow(SW_SHOW);
		break;
	case TERRAIN_EDIT_MODE::SET_HEIGHT:
		m_staticSetTerrainHeightValue.ShowWindow(SW_SHOW);
		m_editSetTerrainHeightValue.ShowWindow(SW_SHOW);
		m_staticSetTerrainHeightValueHelp.ShowWindow(SW_SHOW);
		break;
	case TERRAIN_EDIT_MODE::SMOOTH_HEIGHT:
		m_staticSmoothTerrainHeightBlurDirection.ShowWindow(SW_SHOW);
		m_sliderSmoothTerrainHeightBlurDirection.ShowWindow(SW_SHOW);
		m_editSmoothTerrainHeightBlurDirection.ShowWindow(SW_SHOW);
		break;
	default:
		break;
	}
}


BEGIN_MESSAGE_MAP(CTerrainInspectorFormView, CFormView)
	ON_WM_HSCROLL()
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_BRUSH_TERRAIN, IDC_RADIO_BRUSH_GRASS, &CTerrainInspectorFormView::OnBnClickedTerrainBrushType)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_LAYER0, IDC_RADIO_LAYER4, &CTerrainInspectorFormView::OnBnClickedTerrainLayerToModify)
	ON_CBN_SELCHANGE(IDC_COMBO_TERRAIN_EDIT_MODE, &CTerrainInspectorFormView::OnCbnSelchangeComboTerrainEditMode)
	ON_EN_UPDATE(IDC_EDIT_SET_TERRAIN_HEIGHT_VALUE, &CTerrainInspectorFormView::OnEnUpdateEditSetTerrainHeightValue)
	ON_CBN_SELCHANGE(IDC_COMBO_TERRAIN_DIFFUSE_LAYER, &CTerrainInspectorFormView::OnCbnSelchangeComboTerrainDiffuseLayer)
	ON_CBN_DROPDOWN(IDC_COMBO_TERRAIN_DIFFUSE_LAYER, &CTerrainInspectorFormView::OnCbnDropdownComboTerrainDiffuseLayer)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_TERRAIN_DIFFUSE_LAYER, &CTerrainInspectorFormView::OnBnClickedButtonRemoveTerrainDiffuseLayer)
	ON_CBN_SELCHANGE(IDC_COMBO_TERRAIN_NORMAL_LAYER, &CTerrainInspectorFormView::OnCbnSelchangeComboTerrainNormalLayer)
	ON_CBN_DROPDOWN(IDC_COMBO_TERRAIN_NORMAL_LAYER, &CTerrainInspectorFormView::OnCbnDropdownComboTerrainNormalLayer)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_TERRAIN_NORMAL_LAYER, &CTerrainInspectorFormView::OnBnClickedButtonRemoveTerrainNormalLayer)
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

	// TODO: Add your specialized code here and/or call the base class
	m_currentTerrainBrushType = TERRAIN_BRUSH_TYPE::BRUSH_TERRAIN;

	m_radioBrushTerrain.SetCheck(TRUE);

	m_comboTerrainEditMode.AddString(_T("Raise or Lower Terrain"));
	m_comboTerrainEditMode.AddString(_T("Paint Texture"));
	m_comboTerrainEditMode.AddString(_T("Set Height"));
	m_comboTerrainEditMode.AddString(_T("Smooth Height"));

	// Raise or Lower Terrain 기본 선택
	m_comboTerrainEditMode.SetCurSel(0);

	// 슬라이더 컨트롤 초기화
	m_sliderTerrainBrushSize.SetRange(1, 700);		// -> 슬라이더 틱을 값으로 0.1 ~ 70으로 변환
	m_sliderTerrainBrushSize.SetPos(50);

	m_sliderTerrainBrushOpacity.SetRange(0, 100);		// Paint Texture에서는 0 ~ 255로 매핑하여 R8G8B8A8 UNORM로 변환
	m_sliderTerrainBrushOpacity.SetPos(100);

	m_sliderSmoothTerrainHeightBlurDirection.SetRange(-10, 10);
	m_sliderSmoothTerrainHeightBlurDirection.SetPos(0);
	m_sliderSmoothTerrainHeightBlurDirection.SetTicFreq(1);

	TCHAR str[16];
	{
		int pos = m_sliderTerrainBrushSize.GetPos();
		float value = static_cast<float>(pos) / 10.0f;
		StringCbPrintf(str, sizeof(str), _T("%.1f"), value);
		m_editTerrainBrushSize.SetWindowText(str);
	}
	{
		int pos = m_sliderTerrainBrushOpacity.GetPos();
		StringCbPrintf(str, sizeof(str), _T("%d"), pos);
		m_editTerrainBrushOpacity.SetWindowText(str);
	}
	{
		int pos = m_sliderSmoothTerrainHeightBlurDirection.GetPos();
		float value = static_cast<float>(pos) / 10.0f;
		StringCbPrintf(str, sizeof(str), _T("%.1f"), value);
		m_editSmoothTerrainHeightBlurDirection.SetWindowText(str);
	}
	{
		m_editSetTerrainHeightValue.SetWindowText(_T("0"));
	}

	m_radioLayerModifying0.SetCheck(TRUE);
	m_modifyingLayerIndex = 0;

	// UI 새로고침
	this->OnCbnSelchangeComboTerrainEditMode();
}

void CTerrainInspectorFormView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	const HWND hScrollBar = pScrollBar->GetSafeHwnd();

	TCHAR str[16];
	if (hScrollBar == m_sliderTerrainBrushSize.GetSafeHwnd())
	{
		int pos = m_sliderTerrainBrushSize.GetPos();
		float value = static_cast<float>(pos) / 10.0f;
		StringCbPrintf(str, sizeof(str), _T("%.1f"), value);
		m_editTerrainBrushSize.SetWindowText(str);
	}
	else if (hScrollBar == m_sliderTerrainBrushOpacity.GetSafeHwnd())
	{
		int pos = m_sliderTerrainBrushOpacity.GetPos();
		StringCbPrintf(str, sizeof(str), _T("%d"), pos);
		m_editTerrainBrushOpacity.SetWindowText(str);
	}
	else if (hScrollBar == m_sliderSmoothTerrainHeightBlurDirection.GetSafeHwnd())
	{
		int pos = m_sliderSmoothTerrainHeightBlurDirection.GetPos();
		float value = static_cast<float>(pos) / 10.0f;
		StringCbPrintf(str, sizeof(str), _T("%.1f"), value);
		m_editSmoothTerrainHeightBlurDirection.SetWindowText(str);
	}

	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTerrainInspectorFormView::OnBnClickedTerrainBrushType(UINT id)
{
	switch (id)
	{
	case IDC_RADIO_BRUSH_TERRAIN:
		if (m_currentTerrainBrushType != TERRAIN_BRUSH_TYPE::BRUSH_TERRAIN)
		{
			this->OnShowTerrainBrushUI();
			m_currentTerrainBrushType = TERRAIN_BRUSH_TYPE::BRUSH_TERRAIN;
		}
		break;
	case IDC_RADIO_BRUSH_TREE:
		if (m_currentTerrainBrushType != TERRAIN_BRUSH_TYPE::BRUSH_TREE)
		{
			this->OnHideTerrainBrushUI();
			m_currentTerrainBrushType = TERRAIN_BRUSH_TYPE::BRUSH_TREE;
		}
		break;
	case IDC_RADIO_BRUSH_GRASS:
		if (m_currentTerrainBrushType != TERRAIN_BRUSH_TYPE::BRUSH_GRASS)
		{
			this->OnHideTerrainBrushUI();
			m_currentTerrainBrushType = TERRAIN_BRUSH_TYPE::BRUSH_GRASS;
		}
		m_currentTerrainBrushType = TERRAIN_BRUSH_TYPE::BRUSH_GRASS;
		break;
	default:
		assert(false);
		break;
	}
}

void CTerrainInspectorFormView::OnBnClickedTerrainLayerToModify(UINT id)
{
	switch (id)
	{
	case IDC_RADIO_LAYER0:
		m_modifyingLayerIndex = 0;
		break;
	case IDC_RADIO_LAYER1:
		m_modifyingLayerIndex = 1;
		break;
	case IDC_RADIO_LAYER2:
		m_modifyingLayerIndex = 2;
		break;
	case IDC_RADIO_LAYER3:
		m_modifyingLayerIndex = 3;
		break;
	case IDC_RADIO_LAYER4:
		m_modifyingLayerIndex = 4;
		break;
	default:
		assert(false);
		// m_modifyingLayerIndex = 0;
		break;
	}
}

void CTerrainInspectorFormView::OnCbnSelchangeComboTerrainEditMode()
{
	// TODO: Add your control notification handler code here
	const int sel = m_comboTerrainEditMode.GetCurSel();
	const TERRAIN_EDIT_MODE em = static_cast<TERRAIN_EDIT_MODE>(sel);

	m_currentTerrainEditMode = em;

	this->OnShowTerrainBrushUI();
}

void CTerrainInspectorFormView::OnEnUpdateEditSetTerrainHeightValue()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

	// TODO:  Add your control notification handler code here
	CString strText;
	m_editSetTerrainHeightValue.GetWindowText(strText);

	int nStartChar, nEndChar;
	m_editSetTerrainHeightValue.GetSel(nStartChar, nEndChar);

	int inputValue = _ttoi(strText);

	if (inputValue < 0)
		inputValue = 0;
	else if (inputValue > 65535)
		inputValue = 65535;

	TCHAR str[16];
	StringCbPrintf(str, sizeof(str), _T("%d"), inputValue);

	m_editSetTerrainHeightValue.SetWindowText(str);
	m_editSetTerrainHeightValue.SetSel(nStartChar, nEndChar);  // 원래 커서 위치로 복원
}

void CTerrainInspectorFormView::OnCbnSelchangeComboTerrainDiffuseLayer()
{
	// TODO: Add your control notification handler code here
	const int sel = m_comboTerrainDiffuseLayer.GetCurSel();
	if (sel == CB_ERR)
		return;

	ATVItemTexture* pATVItemTexture = reinterpret_cast<ATVItemTexture*>(m_comboTerrainDiffuseLayer.GetItemData(sel));

	ze::Terrain* pTerrain = this->GetCLVItemToModify()->GetTerrain();
	ze::Texture2D normalMapLayer = pTerrain->GetNormalMapLayer();
	pTerrain->SetTextureLayer(pATVItemTexture->m_texture, std::move(normalMapLayer));
}

void CTerrainInspectorFormView::OnCbnDropdownComboTerrainDiffuseLayer()
{
	// TODO: Add your control notification handler code here
	m_comboTerrainDiffuseLayer.ResetContent();
	auto textureSet = static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().GetTextureSet();

	CAssetTreeView* pATV = static_cast<CMainFrame*>(AfxGetMainWnd())->GetAssetTreeView();
	CTreeCtrl& tc = pATV->GetTreeCtrl();
	TCHAR text[32];
	TVITEM qi;	// Query Info
	qi.mask = TVIF_TEXT | TVIF_PARAM;
	qi.pszText = text;
	qi.cchTextMax = _countof(text);

	auto end = textureSet.cend();
	for (auto iter = textureSet.cbegin(); iter != end; ++iter)
	{
		HTREEITEM hItem = *iter;
		qi.hItem = hItem;

		BOOL ret = tc.GetItem(&qi);
		assert(ret != FALSE);

		int index = m_comboTerrainDiffuseLayer.AddString(text);
		m_comboTerrainDiffuseLayer.SetItemData(index, qi.lParam);
	}
}

void CTerrainInspectorFormView::OnBnClickedButtonRemoveTerrainDiffuseLayer()
{
	// TODO: Add your control notification handler code here
	ze::Terrain* pTerrain = this->GetCLVItemToModify()->GetTerrain();

	ze::Texture2D diffuseMapLayer;	// Empty texture
	ze::Texture2D normalMapLayer = pTerrain->GetNormalMapLayer();

	pTerrain->SetTextureLayer(std::move(diffuseMapLayer), std::move(normalMapLayer));

	m_comboTerrainDiffuseLayer.ResetContent();	// 이거 호출하고 나면 GetCurSel()도 -1 (CB_ERR) 반환함
}

void CTerrainInspectorFormView::OnCbnSelchangeComboTerrainNormalLayer()
{
	// TODO: Add your control notification handler code here
	const int sel = m_comboTerrainNormalLayer.GetCurSel();
	if (sel == CB_ERR)
		return;

	ATVItemTexture* pATVItemTexture = reinterpret_cast<ATVItemTexture*>(m_comboTerrainNormalLayer.GetItemData(sel));

	ze::Terrain* pTerrain = this->GetCLVItemToModify()->GetTerrain();
	ze::Texture2D diffuseMapLayer = pTerrain->GetDiffuseMapLayer();
	pTerrain->SetTextureLayer(std::move(diffuseMapLayer), pATVItemTexture->m_texture);
}

void CTerrainInspectorFormView::OnCbnDropdownComboTerrainNormalLayer()
{
	// TODO: Add your control notification handler code here
	m_comboTerrainNormalLayer.ResetContent();
	auto textureSet = static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().GetTextureSet();

	CAssetTreeView* pATV = static_cast<CMainFrame*>(AfxGetMainWnd())->GetAssetTreeView();
	CTreeCtrl& tc = pATV->GetTreeCtrl();
	TCHAR text[32];
	TVITEM qi;	// Query Info
	qi.mask = TVIF_TEXT | TVIF_PARAM;
	qi.pszText = text;
	qi.cchTextMax = _countof(text);

	auto end = textureSet.cend();
	for (auto iter = textureSet.cbegin(); iter != end; ++iter)
	{
		HTREEITEM hItem = *iter;
		qi.hItem = hItem;

		BOOL ret = tc.GetItem(&qi);
		assert(ret != FALSE);

		int index = m_comboTerrainNormalLayer.AddString(text);
		m_comboTerrainNormalLayer.SetItemData(index, qi.lParam);
	}
}

void CTerrainInspectorFormView::OnBnClickedButtonRemoveTerrainNormalLayer()
{
	// TODO: Add your control notification handler code here
	ze::Terrain* pTerrain = this->GetCLVItemToModify()->GetTerrain();

	ze::Texture2D diffuseMapLayer = pTerrain->GetDiffuseMapLayer();
	ze::Texture2D normalMapLayer;	// Empty texture

	pTerrain->SetTextureLayer(std::move(diffuseMapLayer), std::move(normalMapLayer));
	m_comboTerrainNormalLayer.ResetContent();	// 이거 호출하고 나면 GetCurSel()도 -1 (CB_ERR) 반환함
}
