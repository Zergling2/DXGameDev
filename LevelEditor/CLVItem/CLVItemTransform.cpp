#include "CLVItemTransform.h"
#include "..\MainFrm.h"
#include "..\TransformInspectorFormView.h"

void CLVItemTransform::OnSelect()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());

	// 1. ������Ʈ �ν����� �� �並 Transform �ν����ͷ� ��ü
	CTransformInspectorFormView* pInspector =
		static_cast<CTransformInspectorFormView*>(pMainFrame->SwitchComponentInspectorFormView(RUNTIME_CLASS(CTransformInspectorFormView)));




	pInspector->SetCLVItemToModify(this);		// ���� �� ������ �ݿ��� ���Ͽ�

	// 2. ����� ������Ʈ �ν����� �� �信 �ڽ��� ���� ����
	XMFLOAT3A val;
	CString buf;

	// ������ �� ����
	// Position
	XMStoreFloat3A(&val, m_pTransform->GetLocalPosition());
	buf.Format(_T("%lf"), val.x);
	pInspector->m_positionX.SetWindowTextW(buf);
	buf.Format(_T("%lf"), val.y);
	pInspector->m_positionY.SetWindowTextW(buf);
	buf.Format(_T("%lf"), val.z);
	pInspector->m_positionZ.SetWindowTextW(buf);
	
	// Rotation
	XMStoreFloat3A(&val, ze::Math::QuaternionToEulerNormal(m_pTransform->GetLocalRotation()));
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
	
	// ���� -> ��Ʈ�ѷ� ������Ʈ
	pInspector->UpdateData(FALSE);
}
