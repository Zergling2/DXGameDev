#include "CLVItemEmpty.h"
#include "..\MainFrm.h"
#include "..\EmptyInspectorFormView.h"

void CLVItemEmpty::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// �ν����� �� �� ��ü
	pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CEmptyInspectorFormView));
}
