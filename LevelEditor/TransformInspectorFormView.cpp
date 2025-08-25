#include "TransformInspectorFormView.h"
#include "CLVItem\CLVItemTransform.h"

// CTransformInspectorFormView

IMPLEMENT_DYNCREATE(CTransformInspectorFormView, CFormView)

CTransformInspectorFormView::CTransformInspectorFormView()
	: CFormView(IDD_TRANSFORM_INSPECTOR_FORMVIEW)
	, m_pItem(nullptr)
	, m_positionX()
	, m_positionY()
	, m_positionZ()
	, m_rotationX()
	, m_rotationY()
	, m_rotationZ()
	, m_scaleX()
	, m_scaleY()
	, m_scaleZ()
{
}

CTransformInspectorFormView::~CTransformInspectorFormView()
{
}

void CTransformInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_POSITIONX, m_positionX);
	DDX_Control(pDX, IDC_EDIT_POSITIONY, m_positionY);
	DDX_Control(pDX, IDC_EDIT_POSITIONZ, m_positionZ);
	DDX_Control(pDX, IDC_EDIT_ROTATIONX, m_rotationX);
	DDX_Control(pDX, IDC_EDIT_ROTATIONY, m_rotationY);
	DDX_Control(pDX, IDC_EDIT_ROTATIONZ, m_rotationZ);
	DDX_Control(pDX, IDC_EDIT_SCALEX, m_scaleX);
	DDX_Control(pDX, IDC_EDIT_SCALEY, m_scaleY);
	DDX_Control(pDX, IDC_EDIT_SCALEZ, m_scaleZ);
}

BEGIN_MESSAGE_MAP(CTransformInspectorFormView, CFormView)
	ON_EN_CHANGE(IDC_EDIT_POSITIONX, &CTransformInspectorFormView::OnEnChangeEditPositionX)
	ON_EN_CHANGE(IDC_EDIT_POSITIONY, &CTransformInspectorFormView::OnEnChangeEditPositionY)
	ON_EN_CHANGE(IDC_EDIT_POSITIONZ, &CTransformInspectorFormView::OnEnChangeEditPositionZ)
	ON_EN_CHANGE(IDC_EDIT_ROTATIONX, &CTransformInspectorFormView::OnEnChangeEditRotationX)
	ON_EN_CHANGE(IDC_EDIT_ROTATIONY, &CTransformInspectorFormView::OnEnChangeEditRotationY)
	ON_EN_CHANGE(IDC_EDIT_ROTATIONZ, &CTransformInspectorFormView::OnEnChangeEditRotationZ)
	ON_EN_CHANGE(IDC_EDIT_SCALEX, &CTransformInspectorFormView::OnEnChangeEditScaleX)
	ON_EN_CHANGE(IDC_EDIT_SCALEY, &CTransformInspectorFormView::OnEnChangeEditScaleY)
	ON_EN_CHANGE(IDC_EDIT_SCALEZ, &CTransformInspectorFormView::OnEnChangeEditScaleZ)
END_MESSAGE_MAP()


// CTransformInspectorFormView diagnostics

#ifdef _DEBUG
void CTransformInspectorFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTransformInspectorFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CTransformInspectorFormView::OnEnChangeEditPositionX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here

	// 컨트롤 -> 변수로 값 업데이트
	m_positionX.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A position;
	XMStoreFloat3A(&position, pTransform->GetLocalPosition());
	position.x = m_positionX.GetValue();

	pTransform->SetPosition(position);
}

void CTransformInspectorFormView::OnEnChangeEditPositionY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	// 컨트롤 -> 변수로 값 업데이트
	m_positionY.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A position;
	XMStoreFloat3A(&position, pTransform->GetLocalPosition());
	position.y = m_positionY.GetValue();

	pTransform->SetPosition(position);
}

void CTransformInspectorFormView::OnEnChangeEditPositionZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	// 컨트롤 -> 변수로 값 업데이트
	m_positionZ.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A position;
	XMStoreFloat3A(&position, pTransform->GetLocalPosition());
	position.z = m_positionZ.GetValue();

	pTransform->SetPosition(position);
}

void CTransformInspectorFormView::OnEnChangeEditRotationX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_rotationX.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A rotation;
	XMStoreFloat3A(&rotation, ze::Math::QuaternionToEulerNormal(pTransform->GetLocalRotation()));
	rotation.x = XMConvertToRadians(m_rotationX.GetValue());

	pTransform->SetRotation(rotation);
}

void CTransformInspectorFormView::OnEnChangeEditRotationY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_rotationY.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A rotation;
	XMStoreFloat3A(&rotation, ze::Math::QuaternionToEulerNormal(pTransform->GetLocalRotation()));
	rotation.y = XMConvertToRadians(m_rotationY.GetValue());

	pTransform->SetRotation(rotation);
}

void CTransformInspectorFormView::OnEnChangeEditRotationZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_rotationZ.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A rotation;
	XMStoreFloat3A(&rotation, ze::Math::QuaternionToEulerNormal(pTransform->GetLocalRotation()));
	rotation.z = XMConvertToRadians(m_rotationZ.GetValue());

	pTransform->SetRotation(rotation);
}

void CTransformInspectorFormView::OnEnChangeEditScaleX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_scaleX.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A scale;
	XMStoreFloat3A(&scale, pTransform->GetLocalScale());
	scale.x = m_scaleX.GetValue();

	pTransform->SetScale(scale);
}

void CTransformInspectorFormView::OnEnChangeEditScaleY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_scaleY.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A scale;
	XMStoreFloat3A(&scale, pTransform->GetLocalScale());
	scale.y = m_scaleY.GetValue();

	pTransform->SetScale(scale);
}

void CTransformInspectorFormView::OnEnChangeEditScaleZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_scaleZ.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A scale;
	XMStoreFloat3A(&scale, pTransform->GetLocalScale());
	scale.z = m_scaleZ.GetValue();

	pTransform->SetScale(scale);
}
