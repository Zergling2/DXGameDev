#include "ATVItemEmpty.h"
#include "..\MainFrm.h"
#include "..\ComponentListView.h"
#include "..\EmptyInspectorFormView.h"

void ATVItemEmpty::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// CLV �׸� ��� ����
	pMainFrame->GetComponentListView()->ClearListCtrl();

	// �ν����� �� �信 �� �� �� ����
	pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CEmptyInspectorFormView));
}
