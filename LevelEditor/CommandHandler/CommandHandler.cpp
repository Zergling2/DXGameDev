#include "CommandHandler.h"
#include "..\TerrainGenerationDialog.h"
#include "..\MainFrm.h"
#include "..\HierarchyTreeView.h"
#include "..\HTVItem\HTVItemGameObject.h"
#include "..\Settings.h"
#include <ZergEngine\CoreSystem\Runtime.h>

void On3DObjectTerrain()
{
	CTerrainGenerationDialog dlg;
	INT_PTR ret = dlg.DoModal();

	if (ret != IDOK)
		return;

	TCHAR buf[64];
	StringCbPrintfW(buf,
		sizeof(buf),
		L"Cell Size: %lf, Height Scale: %lf, Row: %u, Col: %u\n",
		dlg.m_cellSize, dlg.m_heightScale, dlg.m_patchCountRow, dlg.m_patchCountColumn
	);
	OutputDebugStringW(buf);
}

void OnGameObjectCreateEmpty()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pHTV = pMainFrame->GetHierarchyTreeView();

	auto& tc = pHTV->GetTreeCtrl();
	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	HTREEITEM hParent;
	if (hSelectedItem)
		hParent = hSelectedItem;
	else
		hParent = tc.GetRootItem();

	PCWSTR gameObjectName = L"New Game Object";
	ze::GameObjectHandle hGameObject = ze::Runtime::GetInstance()->CreateGameObject(gameObjectName);
	if (hParent != tc.GetRootItem())	// 새로 만들어지는 게임 오브젝트가 다른 게임 오브젝트를 부모로 삼는다면
	{
		HTVItemGameObject* pHTVItemGameObject = reinterpret_cast<HTVItemGameObject*>(tc.GetItemData(hParent));
		ze::GameObjectHandle hParentGameObject = pHTVItemGameObject->GetGameObjectHandle();
		ze::GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetParent(&hParentGameObject.ToPtr()->m_transform);
	}

	HTVItemGameObject* pHTVItemGameObject = new HTVItemGameObject(hGameObject);
	const HTREEITEM hNewItem = tc.InsertItem(gameObjectName,
		ZE_ICON_INDEX::GAMEOBJECT_ICON,
		ZE_ICON_INDEX::GAMEOBJECT_ICON,
		hParent
	);
	tc.SetItemData(hNewItem, reinterpret_cast<DWORD_PTR>(pHTVItemGameObject));
	tc.SelectItem(hNewItem);

	// 컴포넌트 리스트 뷰 업데이트
	pHTVItemGameObject->OnSelect();

	tc.EditLabel(hNewItem);
}

void OnGameObjectRename()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pHTV = pMainFrame->GetHierarchyTreeView();

	auto& tc = pHTV->GetTreeCtrl();

	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	if (hSelectedItem == NULL)
		return;

	tc.EditLabel(hSelectedItem);
}

void OnComponentMeshRenderer()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pHTV = pMainFrame->GetHierarchyTreeView();

	auto& tc = pHTV->GetTreeCtrl();

	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	if (hSelectedItem == NULL || hSelectedItem == tc.GetRootItem())
		return;

	const DWORD_PTR data = tc.GetItemData(hSelectedItem);
	HTVItemGameObject* pHTVItemGameObject = reinterpret_cast<HTVItemGameObject*>(data);
	ze::GameObjectHandle hGameObject = pHTVItemGameObject->GetGameObjectHandle();
	ze::GameObject* pGameObject = hGameObject.ToPtr();
	assert(pGameObject != nullptr);

	pGameObject->AddComponent<ze::MeshRenderer>();

	pHTVItemGameObject->OnSelect();
}
