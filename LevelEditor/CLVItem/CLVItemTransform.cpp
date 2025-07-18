#include "CLVItemTransform.h"
#include "..\MainFrm.h"
#include "..\TransformInspectorFormView.h"

void CLVItemTransform::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. 컴포넌트 인스펙터 폼 뷰를 Transform 인스펙터로 교체
	CTransformInspectorFormView* pInspector =
		static_cast<CTransformInspectorFormView*>(pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CTransformInspectorFormView)));

	// 2. 변경된 컴포넌트 인스펙터 폼 뷰에 자신의 정보 세팅
	pInspector->SetCLVItemToModify(this);		// 수정 시 데이터 반영을 위하여

	XMFLOAT3A val;
	CString buf;

	// 변수로 값 세팅
	// Position
	XMStoreFloat3A(&val, m_pTransform->GetLocalPosition());
	
	buf.Format(_T("%lf"), val.x);
	pInspector->m_positionX.SetWindowTextW(buf);
	buf.Format(_T("%lf"), val.y);
	pInspector->m_positionY.SetWindowTextW(buf);
	buf.Format(_T("%lf"), val.z);
	pInspector->m_positionZ.SetWindowTextW(buf);
	
	// Rotation
	XMVECTOR vEuler = ze::Math::QuaternionToEulerNormal(m_pTransform->GetLocalRotation());	// Transform이 쿼터니언을 담고 있으므로
	XMStoreFloat3A(&val, vEuler);
	val.x = XMConvertToDegrees(val.x);	// Radian to degree
	val.y = XMConvertToDegrees(val.y);	// Radian to degree
	val.z = XMConvertToDegrees(val.z);	// Radian to degree
	buf.Format(_T("%lf"), val.x);
	pInspector->m_rotationX.SetWindowTextW(buf);
	buf.Format(_T("%lf"), val.y);
	pInspector->m_rotationY.SetWindowTextW(buf);
	buf.Format(_T("%lf"), val.z);
	pInspector->m_rotationZ.SetWindowTextW(buf);
	
	// Scale
	XMStoreFloat3A(&val, m_pTransform->GetLocalScale());
	
	buf.Format(_T("%lf"), val.x);
	pInspector->m_scaleX.SetWindowTextW(buf);
	buf.Format(_T("%lf"), val.y);
	pInspector->m_scaleY.SetWindowTextW(buf);
	buf.Format(_T("%lf"), val.z);
	pInspector->m_scaleZ.SetWindowTextW(buf);
	
	pInspector->UpdateData(FALSE);	// 변수 -> 컨트롤로 업데이트
}
