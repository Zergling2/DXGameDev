#include "LevelEditor.h"
#include "CLVItem\CLVItemMeshRenderer.h"
#include "ATVItem\ATVItemMesh.h"
#include "ATVItem\ATVItemMaterial.h"
#include "MeshRendererInspectorFormView.h"
#include "AssetTreeView.h"
#include "MainFrm.h"
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MeshRenderer.h>

// CMeshRendererInspectorFormView

IMPLEMENT_DYNCREATE(CMeshRendererInspectorFormView, CFormView)

CMeshRendererInspectorFormView::CMeshRendererInspectorFormView()
	: CFormView(IDD_MESH_RENDERER_INSPECTOR_FORMVIEW)
	, m_pItem(nullptr)
{
}

CMeshRendererInspectorFormView::~CMeshRendererInspectorFormView()
{
}

void CMeshRendererInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESH_RENDERER_CAST_SHADOWS, m_castShadows);
	DDX_Control(pDX, IDC_MESH_RENDERER_RECEIVE_SHADOWS, m_receiveShadows);
	DDX_Control(pDX, IDC_COMBO_SELECT_MESH, m_comboSelectMesh);
	DDX_Control(pDX, IDC_COMBO_SELECT_SUBSET_INDEX, m_comboSelectSubsetIndex);
	DDX_Control(pDX, IDC_COMBO_SELECT_MATERIAL, m_comboSelectMaterial);
}

BEGIN_MESSAGE_MAP(CMeshRendererInspectorFormView, CFormView)
	ON_BN_CLICKED(IDC_MESH_RENDERER_CAST_SHADOWS, &CMeshRendererInspectorFormView::OnBnClickedMeshRendererCastShadows)
	ON_BN_CLICKED(IDC_MESH_RENDERER_RECEIVE_SHADOWS, &CMeshRendererInspectorFormView::OnBnClickedMeshRendererReceiveShadows)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_MESH, &CMeshRendererInspectorFormView::OnCbnSelchangeComboSelectMesh)
	ON_CBN_DROPDOWN(IDC_COMBO_SELECT_MESH, &CMeshRendererInspectorFormView::OnCbnDropdownComboSelectMesh)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE_MESH, &CMeshRendererInspectorFormView::OnBnClickedButtonRemoveMesh)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_SUBSET_INDEX, &CMeshRendererInspectorFormView::OnCbnSelchangeComboSelectSubsetIndex)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_MATERIAL, &CMeshRendererInspectorFormView::OnCbnSelchangeComboSelectMaterial)
	ON_CBN_DROPDOWN(IDC_COMBO_SELECT_MATERIAL, &CMeshRendererInspectorFormView::OnCbnDropdownComboSelectMaterial)
	ON_CBN_CLOSEUP(IDC_COMBO_SELECT_MESH, &CMeshRendererInspectorFormView::OnCbnCloseupComboSelectMesh)
END_MESSAGE_MAP()


// CMeshRendererInspectorFormView diagnostics

#ifdef _DEBUG
void CMeshRendererInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMeshRendererInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CMeshRendererInspectorFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	m_comboSelectSubsetIndex.EnableWindow(FALSE);
	m_comboSelectMaterial.EnableWindow(FALSE);
}



// CMeshRendererInspectorFormView message handlers

void CMeshRendererInspectorFormView::OnBnClickedMeshRendererCastShadows()
{
	// TODO: Add your control notification handler code here
	m_castShadows.UpdateData(TRUE);

	ze::MeshRenderer* pMeshRenderer = this->GetCLVItemToModify()->GetMeshRenderer();
	pMeshRenderer->SetCastShadows(m_castShadows.GetCheck());
}

void CMeshRendererInspectorFormView::OnBnClickedMeshRendererReceiveShadows()
{
	// TODO: Add your control notification handler code here
	m_receiveShadows.UpdateData(TRUE);

	ze::MeshRenderer* pMeshRenderer = this->GetCLVItemToModify()->GetMeshRenderer();
	pMeshRenderer->SetReceiveShadows(m_receiveShadows.GetCheck());
}

void CMeshRendererInspectorFormView::OnCbnSelchangeComboSelectMesh()
{
	// TODO: Add your control notification handler code here

	// 1. 콤보 박스에서 선택된 항목으로 메시 교체
	const int sel = m_comboSelectMesh.GetCurSel();
	if (sel == CB_ERR)
		return;

	if (!m_comboSelectSubsetIndex.IsWindowEnabled())
		m_comboSelectSubsetIndex.EnableWindow(TRUE);
	if (!m_comboSelectMaterial.IsWindowEnabled())
		m_comboSelectMaterial.EnableWindow(TRUE);

	ATVItemMesh* pATVItemMesh = reinterpret_cast<ATVItemMesh*>(m_comboSelectMesh.GetItemData(sel));

	ze::MeshRenderer* pMeshRenderer = this->GetCLVItemToModify()->GetMeshRenderer();
	pMeshRenderer->SetMesh(pATVItemMesh->m_spMesh);

	if (pMeshRenderer->GetMeshPtr() != nullptr)
	{
		// 2. Subset index 표시 (인덱스 문자열 삽입)
		m_comboSelectSubsetIndex.ResetContent();

		const int32_t subsetCount = static_cast<int32_t>(pMeshRenderer->GetSubsetCount());
		for (int32_t i = 0; i < subsetCount; ++i)
		{
			TCHAR buf[8];
			StringCbPrintf(buf, sizeof(buf), _T("%d"), i);

			m_comboSelectSubsetIndex.AddString(buf);
		}

		m_comboSelectSubsetIndex.SetCurSel(0);	// 초기 설정은 0번 인덱스가 설정된 상태로.
	}
}

void CMeshRendererInspectorFormView::OnCbnDropdownComboSelectMesh()
{
	// TODO: Add your control notification handler code here
	m_comboSelectMesh.ResetContent();
	auto meshSet = static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().GetMeshSet();

	CAssetTreeView* pATV = static_cast<CMainFrame*>(AfxGetMainWnd())->GetAssetTreeView();
	CTreeCtrl& tc = pATV->GetTreeCtrl();
	TCHAR text[32];
	TVITEM qi;	// Query Info
	qi.mask = TVIF_TEXT | TVIF_PARAM;
	qi.pszText = text;
	qi.cchTextMax = _countof(text);

	auto end = meshSet.cend();
	for (auto iter = meshSet.cbegin(); iter != end; ++iter)
	{
		HTREEITEM hItem = *iter;
		qi.hItem = hItem;

		BOOL ret = tc.GetItem(&qi);
		ASSERT(ret != FALSE);

		int index = m_comboSelectMesh.AddString(text);
		m_comboSelectMesh.SetItemData(index, qi.lParam);
	}
}


void CMeshRendererInspectorFormView::OnCbnCloseupComboSelectMesh()
{
	// TODO: Add your control notification handler code here
	const int sel = m_comboSelectMesh.GetCurSel();

	if (sel == CB_ERR)
	{
		m_comboSelectSubsetIndex.ResetContent();
		m_comboSelectSubsetIndex.EnableWindow(FALSE);
		m_comboSelectMaterial.ResetContent();
		m_comboSelectMaterial.EnableWindow(FALSE);
	}
}


void CMeshRendererInspectorFormView::OnBnClickedButtonRemoveMesh()
{
	// TODO: Add your control notification handler code here

	// 1. 현재 수정중인 메시 렌더러 컴포넌트의 메시를 제거
	this->GetCLVItemToModify()->GetMeshRenderer()->SetMesh(nullptr);

	// 2. 컨트롤들의 컨텐츠 제거
	m_comboSelectMesh.ResetContent();
	m_comboSelectSubsetIndex.ResetContent();
	m_comboSelectSubsetIndex.EnableWindow(FALSE);
	m_comboSelectMaterial.ResetContent();
	m_comboSelectMaterial.EnableWindow(FALSE);
}

void CMeshRendererInspectorFormView::OnCbnSelchangeComboSelectSubsetIndex()
{
	// TODO: Add your control notification handler code here
	m_comboSelectMaterial.ResetContent();
}

void CMeshRendererInspectorFormView::OnCbnSelchangeComboSelectMaterial()
{
	// TODO: Add your control notification handler code here
	// 
	// 1. 콤보 박스에서 선택된 항목으로 재질 교체
	const int sel = m_comboSelectMaterial.GetCurSel();
	if (sel == CB_ERR)
		return;

	const int subsetSel = m_comboSelectSubsetIndex.GetCurSel();	// subsetSel은 바로 배열 인덱스로도 사용 가능
	if (subsetSel == CB_ERR)
		return;

	ATVItemMaterial* pATVItemMaterial = reinterpret_cast<ATVItemMaterial*>(m_comboSelectMaterial.GetItemData(sel));

	ze::MeshRenderer* pMeshRenderer = this->GetCLVItemToModify()->GetMeshRenderer();
	ASSERT(subsetSel < pMeshRenderer->GetSubsetCount());
	pMeshRenderer->SetMaterial(subsetSel, pATVItemMaterial->GetMaterial());
}

void CMeshRendererInspectorFormView::OnCbnDropdownComboSelectMaterial()
{
	// TODO: Add your control notification handler code here
	m_comboSelectMaterial.ResetContent();
	auto materialSet = static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().GetMaterialSet();

	CAssetTreeView* pATV = static_cast<CMainFrame*>(AfxGetMainWnd())->GetAssetTreeView();
	CTreeCtrl& tc = pATV->GetTreeCtrl();
	TCHAR text[32];
	TVITEM qi;	// Query Info
	qi.mask = TVIF_TEXT | TVIF_PARAM;
	qi.pszText = text;
	qi.cchTextMax = _countof(text);

	auto end = materialSet.cend();
	for (auto iter = materialSet.cbegin(); iter != end; ++iter)
	{
		HTREEITEM hItem = *iter;
		qi.hItem = hItem;

		BOOL ret = tc.GetItem(&qi);
		ASSERT(ret != FALSE);

		int index = m_comboSelectMaterial.AddString(text);
		m_comboSelectMaterial.SetItemData(index, qi.lParam);
	}
}
