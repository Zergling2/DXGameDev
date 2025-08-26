#include "HTVItemGameObject.h"
#include "..\Settings.h"
#include "..\MainFrm.h"
#include "..\ComponentListView.h"
#include "..\CLVItem\CLVItemTransform.h"
#include "..\CLVItem\CLVItemMeshRenderer.h"
#include "..\CLVItem\CLVItemDirectionalLight.h"
#include "..\CLVItem\CLVItemPointLight.h"
#include "..\CLVItem\CLVItemSpotLight.h"
#include "..\CLVItem\CLVItemTerrain.h"
#include "..\CLVItem\CLVItemEmpty.h"
#include "..\TransformInspectorFormView.h"
#include "..\MeshRendererInspectorFormView.h"
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

void HTVItemGameObject::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// Rename �޴� ���� Ȱ��ȭ
	CMenu* pMainFrameMenu = pMainFrame->GetMenu();
	CMenu* pSubMenu = pMainFrameMenu->GetSubMenu(GAMEOBJECT_MENU_INDEX);
	pSubMenu->EnableMenuItem(ID_GAMEOBJECT_RENAME, MF_BYCOMMAND | MF_ENABLED);

	CComponentListView* pComponentListView = pMainFrame->GetComponentListView();
	// ������Ʈ ����Ʈ �� �׸� ��� ����
	pComponentListView->ClearListCtrl();

	CListCtrl& lc = pComponentListView->GetListCtrl();



	ze::GameObject* pGameObject = m_hGameObject.ToPtr();
	assert(pGameObject != nullptr);
	// ������Ʈ ����Ʈ �信 �׸� �߰�
	// Transform CLV Item �߰�
	CLVItemTransform* pCLVItemTransform = new CLVItemTransform(&pGameObject->m_transform);
	int index = lc.InsertItem(lc.GetItemCount(), _T("Transform"), ZE_ICON_INDEX::TRANSFORM_ICON);
	lc.SetItemData(index, reinterpret_cast<DWORD_PTR>(pCLVItemTransform));

	// ������ ������Ʈ�鵵 CLV Item �߰�
	for (ze::IComponent* pComponent : pGameObject->GetComponentList())
	{
		int index;
		switch (pComponent->GetType())
		{
		case ze::COMPONENT_TYPE::MESH_RENDERER:
			index = lc.InsertItem(lc.GetItemCount(), _T("Mesh Renderer"), ZE_ICON_INDEX::MESH_RENDERER_ICON);
			lc.SetItemData(index, reinterpret_cast<DWORD_PTR>(new CLVItemMeshRenderer(static_cast<ze::MeshRenderer*>(pComponent))));
			break;
		case ze::COMPONENT_TYPE::DIRECTIONAL_LIGHT:
			index = lc.InsertItem(lc.GetItemCount(), _T("Directional Light"), ZE_ICON_INDEX::DIRECTIONAL_LIGHT_ICON);
			lc.SetItemData(index, reinterpret_cast<DWORD_PTR>(new CLVItemDirectionalLight(static_cast<ze::DirectionalLight*>(pComponent))));
			break;
		case ze::COMPONENT_TYPE::POINT_LIGHT:
			index = lc.InsertItem(lc.GetItemCount(), _T("Point Light"), ZE_ICON_INDEX::POINT_LIGHT_ICON);
			lc.SetItemData(index, reinterpret_cast<DWORD_PTR>(new CLVItemPointLight(static_cast<ze::PointLight*>(pComponent))));
			break;
		case ze::COMPONENT_TYPE::SPOT_LIGHT:
			index = lc.InsertItem(lc.GetItemCount(), _T("Spot Light"), ZE_ICON_INDEX::SPOT_LIGHT_ICON);
			lc.SetItemData(index, reinterpret_cast<DWORD_PTR>(new CLVItemSpotLight(static_cast<ze::SpotLight*>(pComponent))));
			break;
		case ze::COMPONENT_TYPE::TERRAIN:
			index = lc.InsertItem(lc.GetItemCount(), _T("Terrain"), ZE_ICON_INDEX::TERRAIN_ICON);
			lc.SetItemData(index, reinterpret_cast<DWORD_PTR>(new CLVItemTerrain(static_cast<ze::Terrain*>(pComponent))));
			break;
		default:
			// �������� ������Ʈ�� ����
			break;
		}
	}

	pCLVItemTransform->OnSelect();
}
