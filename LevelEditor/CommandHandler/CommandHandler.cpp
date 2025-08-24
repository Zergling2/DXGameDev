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

	// Create Terrain Object
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pHTV = pMainFrame->GetHierarchyTreeView();

	auto& tc = pHTV->GetTreeCtrl();
	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	HTREEITEM hParent;
	if (hSelectedItem)
		hParent = hSelectedItem;
	else
		hParent = tc.GetRootItem();

	ze::GameObjectHandle hGameObject = ze::Runtime::GetInstance()->CreateGameObject(L"New Terrain Object");
	ze::GameObject* pGameObject = hGameObject.ToPtr();
	if (hParent != tc.GetRootItem())	// 새로 만들어지는 게임 오브젝트가 다른 게임 오브젝트를 부모로 삼는다면
	{
		// 런타임 게임 오브젝트 계층 구조 설정 (SetParent)
		HTVItemGameObject* pHTVItemGameObject = reinterpret_cast<HTVItemGameObject*>(tc.GetItemData(hParent));
		ze::GameObjectHandle hParentGameObject = pHTVItemGameObject->GetGameObjectHandle();
		pGameObject->m_transform.SetParent(&hParentGameObject.ToPtr()->m_transform);
	}

	// Terrain 컴포넌트 추가 및 지형 생성
	ze::ComponentHandle<ze::Terrain> hTerrain = pGameObject->AddComponent<ze::Terrain>();
	ze::Texture2D heightMap;
	const size_t cellCount = (dlg.m_patchCountRow * ze::CELLS_PER_TERRAIN_PATCH + 1) * (dlg.m_patchCountColumn * ze::CELLS_PER_TERRAIN_PATCH + 1);
	uint16_t* pHeightData = new uint16_t[cellCount];
	ZeroMemory(pHeightData, sizeof(uint16_t) * cellCount);
	ze::ResourceLoader::GetInstance()->CreateHeightMapFromRawData(
		heightMap,
		pHeightData,
		SIZE{ static_cast<LONG>(dlg.m_patchCountColumn * ze::CELLS_PER_TERRAIN_PATCH + 1), static_cast<LONG>(dlg.m_patchCountRow * ze::CELLS_PER_TERRAIN_PATCH + 1) }
	);
	delete[] pHeightData;
	hTerrain.ToPtr()->SetHeightMap(heightMap, dlg.m_cellSize, dlg.m_heightScale);

	HTVItemGameObject* pHTVItemGameObject = new HTVItemGameObject(hGameObject);
	const HTREEITEM hNewItem = tc.InsertItem(L"New Terrain Object",
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

	ze::GameObjectHandle hGameObject = ze::Runtime::GetInstance()->CreateGameObject(L"New Game Object");
	if (hParent != tc.GetRootItem())	// 새로 만들어지는 게임 오브젝트가 다른 게임 오브젝트를 부모로 삼는다면
	{
		// 런타임 게임 오브젝트 계층 구조 설정 (SetParent)
		HTVItemGameObject* pHTVItemGameObject = reinterpret_cast<HTVItemGameObject*>(tc.GetItemData(hParent));
		ze::GameObjectHandle hParentGameObject = pHTVItemGameObject->GetGameObjectHandle();
		ze::GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetParent(&hParentGameObject.ToPtr()->m_transform);
	}

	HTVItemGameObject* pHTVItemGameObject = new HTVItemGameObject(hGameObject);
	const HTREEITEM hNewItem = tc.InsertItem(L"New Game Object",
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

	// 컴포넌트 리스트 갱신
	pHTVItemGameObject->OnSelect();
}
