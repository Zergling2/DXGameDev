#include "CommandHandler.h"
#include "..\LevelEditor.h"
#include "..\AssetManager.h"
#include "..\TerrainGenerationDialog.h"
#include "..\LightingSettingsDialog.h"
#include "..\MainFrm.h"
#include "..\HierarchyTreeView.h"
#include "..\AssetTreeView.h"
#include "..\HTVItem\HTVItemGameObject.h"
#include "..\ATVItem\ATVItemInterface.h"
#include "..\ATVItem\ATVItemFolder.h"
#include "..\ATVItem\ATVItemMaterial.h"
#include "..\ATVItem\ATVItemTexture.h"
#include "..\ATVItem\ATVItemMesh.h"
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

HTREEITEM CreateEmptyGameObject()
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

		ze::GameObject* pGameObject = hGameObject.ToPtr();
		ze::GameObjectHandle hParentGameObject = pHTVItemGameObject->GetGameObjectHandle();

		pGameObject->m_transform.SetParent(&hParentGameObject.ToPtr()->m_transform);
	}

	HTVItemGameObject* pHTVItemGameObject = new HTVItemGameObject(hGameObject);
	const HTREEITEM hNewItem = tc.InsertItem(L"New Game Object",
		ZE_ICON_INDEX::GAMEOBJECT_ICON,
		ZE_ICON_INDEX::GAMEOBJECT_ICON,
		hParent
	);

	tc.SetItemData(hNewItem, reinterpret_cast<DWORD_PTR>(pHTVItemGameObject));

	return hNewItem;
}

void OnGameObjectCreateEmpty()
{
	const HTREEITEM hNewItem = CreateEmptyGameObject();

	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pHTV = pMainFrame->GetHierarchyTreeView();
	auto& tc = pHTV->GetTreeCtrl();

	tc.SelectItem(hNewItem);

	// ������Ʈ ����Ʈ �� ������Ʈ
	HTVItemGameObject* pHTVItemGameObject = reinterpret_cast<HTVItemGameObject*>(pHTV->GetTreeCtrl().GetItemData(hNewItem));
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

void OnLightDirectionalLight()
{
	HTREEITEM hNewItem = CreateEmptyGameObject();

	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pHTV = pMainFrame->GetHierarchyTreeView();
	CTreeCtrl& tc = pHTV->GetTreeCtrl();

	HTVItemGameObject* pHTVItemGameObject = reinterpret_cast<HTVItemGameObject*>(pHTV->GetTreeCtrl().GetItemData(hNewItem));

	// Directional Light ������Ʈ �߰�
	ze::GameObject* pGameObject = pHTVItemGameObject->GetGameObjectHandle().ToPtr();
	pGameObject->AddComponent<ze::DirectionalLight>();

	tc.SelectItem(hNewItem);
	
	// ������Ʈ ����Ʈ �� ������Ʈ
	pHTVItemGameObject->OnSelect();
}

void OnLightPointLight()
{
	HTREEITEM hNewItem = CreateEmptyGameObject();

	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pHTV = pMainFrame->GetHierarchyTreeView();
	CTreeCtrl& tc = pHTV->GetTreeCtrl();

	HTVItemGameObject* pHTVItemGameObject = reinterpret_cast<HTVItemGameObject*>(pHTV->GetTreeCtrl().GetItemData(hNewItem));

	// Point Light ������Ʈ �߰�
	ze::GameObject* pGameObject = pHTVItemGameObject->GetGameObjectHandle().ToPtr();
	pGameObject->AddComponent<ze::PointLight>();

	tc.SelectItem(hNewItem);

	// ������Ʈ ����Ʈ �� ������Ʈ
	pHTVItemGameObject->OnSelect();
}

void OnLightSpotLight()
{
	HTREEITEM hNewItem = CreateEmptyGameObject();

	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	auto pHTV = pMainFrame->GetHierarchyTreeView();
	CTreeCtrl& tc = pHTV->GetTreeCtrl();

	HTVItemGameObject* pHTVItemGameObject = reinterpret_cast<HTVItemGameObject*>(pHTV->GetTreeCtrl().GetItemData(hNewItem));

	// Point Light ������Ʈ �߰�
	ze::GameObject* pGameObject = pHTVItemGameObject->GetGameObjectHandle().ToPtr();
	pGameObject->AddComponent<ze::SpotLight>();

	tc.SelectItem(hNewItem);

	// ������Ʈ ����Ʈ �� ������Ʈ
	pHTVItemGameObject->OnSelect();
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

	// ���� ���õ� �׸��� �θ�� �� �����׸� ����
	HTREEITEM hParent = hSelectedItem;

	ATVItemMaterial* pATVItemMaterial = new ATVItemMaterial(ze::ResourceLoader::GetInstance()->CreateMaterial());
	const HTREEITEM hNewItem = tc.InsertItem(L"New Material",
		ZE_ICON_INDEX::MATERIAL_ICON,
		ZE_ICON_INDEX::MATERIAL_ICON,
		hParent
	);
	// Asset Manager�� �ڵ� ���
	static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().AddATVItemMaterialHandle(hNewItem);

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

	if (fd.DoModal() != IDOK)
		return;

	CString filePath = fd.GetPathName(); // ���õ� ���� ���
	ATVItemTexture* pATVItemTexture = new ATVItemTexture();
	pATVItemTexture->m_texture = ze::ResourceLoader::GetInstance()->LoadTexture2D(filePath.GetString(), true);

	HTREEITEM hParent = hSelectedItem;

	const HTREEITEM hNewItem = tc.InsertItem(L"New Texture",
		ZE_ICON_INDEX::TEXTURE_ICON,
		ZE_ICON_INDEX::TEXTURE_ICON,
		hParent
	);
	// Asset Manager�� �ڵ� ���
	static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().AddATVItemTextureHandle(hNewItem);
	tc.SetItemData(hNewItem, reinterpret_cast<DWORD_PTR>(pATVItemTexture));
	tc.SelectItem(hNewItem);

	// ������Ʈ ����Ʈ �� ������Ʈ
	pATVItemTexture->OnSelect();

	tc.EditLabel(hNewItem);
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

	if (fd.DoModal() != IDOK)
		return;

	CString filePath = fd.GetPathName(); // ���õ� ���� ���
	char path[MAX_PATH];
	ze::Helper::ConvertUTF16ToUTF8(filePath.GetString(), path, sizeof(path));
	// std::vector<std::shared_ptr<ze::Mesh>> meshes = ze::ResourceLoader::GetInstance()->LoadWavefrontOBJ(filePath.GetString());
	std::vector<std::shared_ptr<ze::Mesh>> meshes = ze::ResourceLoader::GetInstance()->LoadMesh(path);
	HTREEITEM hParent = hSelectedItem;

	// ù ��° �޽ø� ���õ� ���·� ����
	ATVItemMesh* pATVItemMesh = new ATVItemMesh();
	pATVItemMesh->m_spMesh = meshes[0];

	const HTREEITEM hNewItem = tc.InsertItem(pATVItemMesh->m_spMesh->GetName(),
		ZE_ICON_INDEX::MESH_ICON,
		ZE_ICON_INDEX::MESH_ICON,
		hParent
	);
	// Asset Manager�� �ڵ� ���
	static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().AddATVItemMeshHandle(hNewItem);
	tc.SetItemData(hNewItem, reinterpret_cast<DWORD_PTR>(pATVItemMesh));
	tc.SelectItem(hNewItem);

	// ������Ʈ ����Ʈ �� ������Ʈ
	pATVItemMesh->OnSelect();

	// ������ �޽õ鵵 �׸� ����
	for (size_t i = 1; i < meshes.size(); ++i)
	{
		ATVItemMesh* pATVItemMesh = new ATVItemMesh();
		pATVItemMesh->m_spMesh = meshes[i];

		const HTREEITEM hNewItem = tc.InsertItem(meshes[i]->GetName(),
			ZE_ICON_INDEX::MESH_ICON,
			ZE_ICON_INDEX::MESH_ICON,
			hParent
		);
		// Asset Manager�� �ڵ� ���
		static_cast<CLevelEditorApp*>(AfxGetApp())->GetAssetManager().AddATVItemMeshHandle(hNewItem);
		tc.SetItemData(hNewItem, reinterpret_cast<DWORD_PTR>(pATVItemMesh));
	}
}

void OnRenderingLighting()
{
	CLightingSettingsDialog dlg;
	INT_PTR ret = dlg.DoModal();
}
