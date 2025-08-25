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
	if (hParent != tc.GetRootItem())	// ���� ��������� ���� ������Ʈ�� �ٸ� ���� ������Ʈ�� �θ�� ��´ٸ�
	{
		// ��Ÿ�� ���� ������Ʈ ���� ���� ���� (SetParent)
		HTVItemGameObject* pHTVItemGameObject = reinterpret_cast<HTVItemGameObject*>(tc.GetItemData(hParent));
		ze::GameObjectHandle hParentGameObject = pHTVItemGameObject->GetGameObjectHandle();
		pGameObject->m_transform.SetParent(&hParentGameObject.ToPtr()->m_transform);
	}

	// Terrain ������Ʈ �߰� �� ���� ����
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

	// ������Ʈ ����Ʈ �� ������Ʈ
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
	if (hParent != tc.GetRootItem())	// ���� ��������� ���� ������Ʈ�� �ٸ� ���� ������Ʈ�� �θ�� ��´ٸ�
	{
		// ��Ÿ�� ���� ������Ʈ ���� ���� ���� (SetParent)
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

	// ������Ʈ ����Ʈ �� ������Ʈ
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

	// ������Ʈ ����Ʈ ����
	pHTVItemGameObject->OnSelect();
}


// �ƹ��͵� ���õ��� ���� ��쿡�� Folder�� ���� �����ϰ� �Ѵ�.
// ������ �ƴ� ��� ���µ��� ��Ʈ ���丮�� ��ġ���� ���ϰ� �Ѵ�.
// ...

void OnCreateAssetFolder()
{
	// ���� ���õ� �׸��� ���ų� ������ ��쿡�� ���� �׸����� ������ �����Ѵ�.
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pATV = pMainFrame->GetAssetTreeView();

	auto& tc = pATV->GetTreeCtrl();
	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	HTREEITEM hParent;
	if (hSelectedItem)
	{
		IATVItem* pATVItem = reinterpret_cast<IATVItem*>(tc.GetItemData(hSelectedItem));
		if (pATVItem->GetType() != ATV_ITEM_TYPE::FOLDER)	// ���õ� �׸��� ������ �ƴ� ��쿡�� ���� �׸����� ������ ������� �ʴ´�.
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

	// ������Ʈ ����Ʈ �� ������Ʈ
	pATVItemFolder->OnSelect();

	tc.EditLabel(hNewItem);
}

void OnCreateAssetMaterial()
{
	// ���� ���� Ʈ���信�� ���õ� �׸��� ������ ��쿡�� ���� �׸����� Material ������ ���
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pATV = pMainFrame->GetAssetTreeView();

	auto& tc = pATV->GetTreeCtrl();
	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	if (hSelectedItem == NULL)	// ���� ���õ� �׸�(����)�� ���� ���
		return;

	IATVItem* pATVItem = reinterpret_cast<IATVItem*>(tc.GetItemData(hSelectedItem));
	if (pATVItem->GetType() != ATV_ITEM_TYPE::FOLDER)	// ���õ� �׸��� ������ �ƴ� ��쿡�� ���� �׸����� ������ ������� �ʴ´�.
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

	// ������Ʈ ����Ʈ �� ������Ʈ
	pATVItemMaterial->OnSelect();

	tc.EditLabel(hNewItem);
}

void OnCreateAssetTexture()
{
	// ���� ���� Ʈ���信�� ���õ� �׸��� ������ ��쿡�� ���� �׸����� Material ������ ���
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pATV = pMainFrame->GetAssetTreeView();

	auto& tc = pATV->GetTreeCtrl();
	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	if (hSelectedItem == NULL)	// ���� ���õ� �׸�(����)�� ���� ���
		return;

	IATVItem* pATVItem = reinterpret_cast<IATVItem*>(tc.GetItemData(hSelectedItem));
	if (pATVItem->GetType() != ATV_ITEM_TYPE::FOLDER)	// ���õ� �׸��� ������ �ƴ� ��쿡�� ���� �׸����� ������ ������� �ʴ´�.
		return;

	PCTSTR filter = _T("Image File (*.png; *.jpg; *.jpeg; *.tga; *.dds)|*.png;*.jpg;*.jpeg;*.tga;*.dds|")
		_T("PNG File (*.png)|*.png|")
		_T("JPEG File (*.jpg; *.jpeg)|*.jpg;*.jpeg|")
		_T("TGA File (*.tga)|*.tga|")
		_T("DDS File (*.dds)|*.dds||");

	CFileDialog fd(
		TRUE,			// TRUE = Open, FALSE = Save
		nullptr,		// lpszDefExt (Ȯ���� ������� ���� �� �ڵ����� append �� Ȯ����)
		nullptr,		// lpszFileName (�⺻ ���ϸ�)
		OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING,
		filter
	);

	if (fd.DoModal() == IDOK)
	{
		CString filePath = fd.GetPathName(); // ���õ� ���� ���
		ze::ResourceLoader::GetInstance()->LoadTexture2D(filePath.GetString(), true);
	}
}

void OnCreateAssetWavefrontOBJ()
{
	// ���� ���� Ʈ���信�� ���õ� �׸��� ������ ��쿡�� ���� �׸����� Material ������ ���
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pATV = pMainFrame->GetAssetTreeView();

	auto& tc = pATV->GetTreeCtrl();
	const HTREEITEM hSelectedItem = tc.GetSelectedItem();
	if (hSelectedItem == NULL)	// ���� ���õ� �׸�(����)�� ���� ���
		return;

	IATVItem* pATVItem = reinterpret_cast<IATVItem*>(tc.GetItemData(hSelectedItem));
	if (pATVItem->GetType() != ATV_ITEM_TYPE::FOLDER)	// ���õ� �׸��� ������ �ƴ� ��쿡�� ���� �׸����� ������ ������� �ʴ´�.
		return;

	PCTSTR filter = _T("Wavefront OBJ File (*.obj)|*.obj||");

	CFileDialog fd(
		TRUE,			// TRUE = Open, FALSE = Save
		nullptr,		// lpszDefExt (Ȯ���� ������� ���� �� �ڵ����� append �� Ȯ����)
		nullptr,		// lpszFileName (�⺻ ���ϸ�)
		OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING,
		filter
	);

	if (fd.DoModal() == IDOK)
	{
		CString filePath = fd.GetPathName(); // ���õ� ���� ���

		// ze::ResourceLoader::GetInstance()->LoadTexture()
	}
}
