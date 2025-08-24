#include "ATVItemFolder.h"
#include "..\MainFrm.h"
#include "..\ComponentListView.h"
#include "..\EmptyInspectorFormView.h"

void ATVItemFolder::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// CLV �׸� ��� ����
	pMainFrame->GetComponentListView()->ClearListCtrl();

	// CIFV�� �� �� �� ����
	pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CEmptyInspectorFormView));
}
