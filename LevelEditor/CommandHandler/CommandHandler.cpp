#include "CommandHandler.h"
#include "..\framework.h"
#include "..\TerrainGenerationDialog.h"
#include "..\MainFrm.h"
#include "..\HierarchyTreeView.h"
#include "..\AssetTreeView.h"
#include "..\HTVItem\HTVItemGameObject.h"
#include "..\ATVItem\ATVItemInterface.h"
#include "..\ATVItem\ATVItemFolder.h"
#include "..\ATVItem\ATVItemMaterial.h"
#include "..\Settings.h"

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


// 아무것도 선택되지 않은 경우에는 Folder만 생성 가능하게 한다.
// 폴더가 아닌 모든 에셋들은 루트 디렉토리에 위치하지 못하게 한다.
// ...

void OnCreateAssetFolder()
{
	// 현재 선택된 항목이 없거나 폴더인 경우에만 하위 항목으로 폴더를 생성한다.
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pATV = pMainFrame->GetAssetTreeView();

	auto& tc = pATV->GetTreeCtrl();
	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	HTREEITEM hParent;
	if (hSelectedItem)
	{
		IATVItem* pATVItem = reinterpret_cast<IATVItem*>(tc.GetItemData(hSelectedItem));
		if (pATVItem->GetType() != ATV_ITEM_TYPE::FOLDER)	// 선택된 항목이 폴더가 아닌 경우에는 하위 항목으로 생성을 허용하지 않는다.
			return;

		hParent = hSelectedItem;
	}
	else
	{
		hParent = TVI_ROOT;
	}

	ATVItemFolder* pATVItemFolder = new ATVItemFolder();
	const HTREEITEM hNewItem = tc.InsertItem(L"New Folder",
		ZE_ICON_INDEX::FOLDER_CLOSED_ICON,
		ZE_ICON_INDEX::FOLDER_CLOSED_ICON,
		hParent
	);
	tc.SetItemData(hNewItem, reinterpret_cast<DWORD_PTR>(pATVItemFolder));
	tc.SelectItem(hNewItem);

	// 컴포넌트 리스트 뷰 업데이트
	pATVItemFolder->OnSelect();

	tc.EditLabel(hNewItem);
}

void OnCreateAssetMaterial()
{
	// 현재 에셋 트리뷰에서 선택된 항목이 폴더인 경우에만 하위 항목으로 Material 생성을 허용
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pATV = pMainFrame->GetAssetTreeView();

	auto& tc = pATV->GetTreeCtrl();
	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	if (hSelectedItem == NULL)	// 현재 선택된 항목(폴더)이 없는 경우
		return;

	IATVItem* pATVItem = reinterpret_cast<IATVItem*>(tc.GetItemData(hSelectedItem));
	if (pATVItem->GetType() != ATV_ITEM_TYPE::FOLDER)	// 선택된 항목이 폴더가 아닌 경우에는 하위 항목으로 생성을 허용하지 않는다.
		return;

	HTREEITEM hParent = hSelectedItem;

	ATVItemMaterial* pATVItemMaterial = new ATVItemMaterial();
	pATVItemMaterial->m_spMaterial = ze::ResourceLoader::GetInstance()->CreateMaterial();
	const HTREEITEM hNewItem = tc.InsertItem(L"New Material",
		ZE_ICON_INDEX::MATERIAL_ICON,
		ZE_ICON_INDEX::MATERIAL_ICON,
		hParent
	);
	tc.SetItemData(hNewItem, reinterpret_cast<DWORD_PTR>(pATVItemMaterial));
	tc.SelectItem(hNewItem);

	// 컴포넌트 리스트 뷰 업데이트
	pATVItemMaterial->OnSelect();

	tc.EditLabel(hNewItem);
}

void OnCreateAssetTexture()
{
	// 현재 에셋 트리뷰에서 선택된 항목이 폴더인 경우에만 하위 항목으로 Material 생성을 허용
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pATV = pMainFrame->GetAssetTreeView();

	auto& tc = pATV->GetTreeCtrl();
	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	if (hSelectedItem == NULL)	// 현재 선택된 항목(폴더)이 없는 경우
		return;

	IATVItem* pATVItem = reinterpret_cast<IATVItem*>(tc.GetItemData(hSelectedItem));
	if (pATVItem->GetType() != ATV_ITEM_TYPE::FOLDER)	// 선택된 항목이 폴더가 아닌 경우에는 하위 항목으로 생성을 허용하지 않는다.
		return;

	PCTSTR filter = _T("Image File (*.png; *.jpg; *.jpeg; *.tga; *.dds)|*.png;*.jpg;*.jpeg;*.tga;*.dds|")
		_T("PNG File (*.png)|*.png|")
		_T("JPEG File (*.jpg; *.jpeg)|*.jpg;*.jpeg|")
		_T("TGA File (*.tga)|*.tga|")
		_T("DDS File (*.dds)|*.dds||");

	CFileDialog fd(
		TRUE,			// TRUE = Open, FALSE = Save
		nullptr,		// lpszDefExt (확장자 명시하지 않을 시 자동으로 append 될 확장자)
		nullptr,		// lpszFileName (기본 파일명)
		OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING,
		filter
	);

	if (fd.DoModal() == IDOK)
	{
		CString filePath = fd.GetPathName(); // 선택된 파일 경로
		ze::ResourceLoader::GetInstance()->LoadTexture2D(filePath.GetString(), true);
	}
}

void OnCreateAssetWavefrontOBJ()
{
	// 현재 에셋 트리뷰에서 선택된 항목이 폴더인 경우에만 하위 항목으로 Material 생성을 허용
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pATV = pMainFrame->GetAssetTreeView();

	auto& tc = pATV->GetTreeCtrl();
	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	if (hSelectedItem == NULL)	// 현재 선택된 항목(폴더)이 없는 경우
		return;

	IATVItem* pATVItem = reinterpret_cast<IATVItem*>(tc.GetItemData(hSelectedItem));
	if (pATVItem->GetType() != ATV_ITEM_TYPE::FOLDER)	// 선택된 항목이 폴더가 아닌 경우에는 하위 항목으로 생성을 허용하지 않는다.
		return;

	PCTSTR filter = _T("Wavefront OBJ File (*.obj)|*.obj||");

	CFileDialog fd(
		TRUE,			// TRUE = Open, FALSE = Save
		nullptr,		// lpszDefExt (확장자 명시하지 않을 시 자동으로 append 될 확장자)
		nullptr,		// lpszFileName (기본 파일명)
		OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING,
		filter
	);

	if (fd.DoModal() == IDOK)
	{
		CString filePath = fd.GetPathName(); // 선택된 파일 경로

		// ze::ResourceLoader::GetInstance()->LoadTexture()
	}
}
