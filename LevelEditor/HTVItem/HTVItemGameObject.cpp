#include "HTVItemGameObject.h"
#include "..\Settings.h"
#include "..\MainFrm.h"
#include "..\ComponentListView.h"
#include "..\CLVItem\CLVItemTransform.h"
#include "..\CLVItem\CLVItemMeshRenderer.h"
#include "..\CLVItem\CLVItemEmpty.h"
#include "..\TransformInspectorFormView.h"
#include "..\MeshRendererInspectorFormView.h"
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

void HTVItemGameObject::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// Rename 메뉴 선택 활성화
	CMenu* pMainFrameMenu = pMainFrame->GetMenu();
	CMenu* pSubMenu = pMainFrameMenu->GetSubMenu(GAMEOBJECT_MENU_INDEX);
	pSubMenu->EnableMenuItem(ID_GAMEOBJECT_RENAME, MF_BYCOMMAND | MF_ENABLED);

	CComponentListView* pComponentListView = pMainFrame->GetComponentListView();
	// 컴포넌트 리스트 뷰 항목 모두 제거
	pComponentListView->ClearListCtrl();

	CListCtrl& lc = pComponentListView->GetListCtrl();



	ze::GameObject* pGameObject = m_hGameObject.ToPtr();
	assert(pGameObject != nullptr);
	// 컴포넌트 리스트 뷰에 항목 추가
	// Transform CLV Item 추가
	CLVItemTransform* pCLVItemTransform = new CLVItemTransform(&pGameObject->m_transform);
	int index = lc.InsertItem(lc.GetItemCount(), _T("Transform"), ZE_ICON_INDEX::TRANSFORM_ICON);
	lc.SetItemData(index, reinterpret_cast<DWORD_PTR>(pCLVItemTransform));

	// 나머지 컴포넌트들도 CLV Item 추가
	for (ze::IComponent* pComponent : pGameObject->GetComponentList())
	{
		switch (pComponent->GetType())
		{
		case ze::COMPONENT_TYPE::MESH_RENDERER:
		{
			CLVItemMeshRenderer* pCLVItemMeshRenderer = new CLVItemMeshRenderer(static_cast<ze::MeshRenderer*>(pComponent));
			int index = lc.InsertItem(lc.GetItemCount(), _T("Mesh Renderer"), ZE_ICON_INDEX::MESH_RENDERER_ICON);
			lc.SetItemData(index, reinterpret_cast<DWORD_PTR>(pCLVItemMeshRenderer));
		}
			break;
		default:
			// 나머지는 컴포넌트는 보류
			break;
		}
	}

	pCLVItemTransform->OnSelect();
}
