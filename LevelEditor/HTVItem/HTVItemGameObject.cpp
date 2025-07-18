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
			// �������� ������Ʈ�� ����
			break;
		}
	}

	pCLVItemTransform->OnSelect();
}
