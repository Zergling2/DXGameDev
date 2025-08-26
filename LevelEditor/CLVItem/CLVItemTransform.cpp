#include "CLVItemTransform.h"
#include "..\MainFrm.h"
#include "..\TransformInspectorFormView.h"

void CLVItemTransform::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. 인스펙터 폼 뷰 교체
	CTransformInspectorFormView* pInspector =
		static_cast<CTransformInspectorFormView*>(pMainFrame->SwitchInspectorFormView(RUNTIME_CLASS(CTransformInspectorFormView)));


	pInspector->SetCLVItemToModify(this);		// 수정 시 데이터 반영을 위하여

	// 2. 변경된 인스펙터 폼 뷰에 정보 세팅
	XMFLOAT3A val;
	TCHAR buf[32];

	// 변수로 값 세팅
	// Position
	XMStoreFloat3A(&val, m_pTransform->GetLocalPosition());
	StringCbPrintf(buf, sizeof(buf), _T("%f"), val.x);
	pInspector->m_editPositionX.SetWindowTextW(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), val.y);
	pInspector->m_editPositionY.SetWindowTextW(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), val.z);
	pInspector->m_editPositionZ.SetWindowTextW(buf);
	
	// Rotation
	XMStoreFloat3A(&val, ze::Math::QuaternionToEulerNormal(m_pTransform->GetLocalRotation()));
	val.x = XMConvertToDegrees(val.x);	// Radian to degree
	val.y = XMConvertToDegrees(val.y);	// Radian to degree
	val.z = XMConvertToDegrees(val.z);	// Radian to degree
	StringCbPrintf(buf, sizeof(buf), _T("%f"), val.x);
	pInspector->m_editRotationX.SetWindowTextW(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), val.y);
	pInspector->m_editRotationY.SetWindowTextW(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), val.z);
	pInspector->m_editRotationZ.SetWindowTextW(buf);
	
	// Scale
	XMStoreFloat3A(&val, m_pTransform->GetLocalScale());
	StringCbPrintf(buf, sizeof(buf), _T("%f"), val.x);
	pInspector->m_editScaleX.SetWindowTextW(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), val.y);
	pInspector->m_editScaleY.SetWindowTextW(buf);
	StringCbPrintf(buf, sizeof(buf), _T("%f"), val.z);
	pInspector->m_editScaleZ.SetWindowTextW(buf);
	
	// 변수 -> 컨트롤로 업데이트
	pInspector->UpdateData(FALSE);
}
