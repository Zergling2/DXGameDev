#include "CLVItemEmpty.h"
#include "..\MainFrm.h"
#include "..\EmptyInspectorFormView.h"

void CLVItemEmpty::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// ������Ʈ �ν����� �� �� ��ü
	pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CEmptyInspectorFormView));
}
