#include "TransformInspectorFormView.h"
#include "CLVItem\CLVItemTransform.h"
#include <ZergEngine\CoreSystem\Math.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Transform.h>

// CTransformInspectorFormView

IMPLEMENT_DYNCREATE(CTransformInspectorFormView, CFormView)

CTransformInspectorFormView::CTransformInspectorFormView()
	: CFormView(IDD_TRANSFORM_INSPECTOR_FORMVIEW)
	, m_pItem(nullptr)
	, m_editPositionX()
	, m_editPositionY()
	, m_editPositionZ()
	, m_editRotationX()
	, m_editRotationY()
	, m_editRotationZ()
	, m_editScaleX()
	, m_editScaleY()
	, m_editScaleZ()
{
}

CTransformInspectorFormView::~CTransformInspectorFormView()
{
}

void CTransformInspectorFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_POSITIONX, m_editPositionX);
	DDX_Control(pDX, IDC_EDIT_POSITIONY, m_editPositionY);
	DDX_Control(pDX, IDC_EDIT_POSITIONZ, m_editPositionZ);
	DDX_Control(pDX, IDC_EDIT_ROTATIONX, m_editRotationX);
	DDX_Control(pDX, IDC_EDIT_ROTATIONY, m_editRotationY);
	DDX_Control(pDX, IDC_EDIT_ROTATIONZ, m_editRotationZ);
	DDX_Control(pDX, IDC_EDIT_SCALEX, m_editScaleX);
	DDX_Control(pDX, IDC_EDIT_SCALEY, m_editScaleY);
	DDX_Control(pDX, IDC_EDIT_SCALEZ, m_editScaleZ);
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
	m_editPositionX.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A position;
	XMStoreFloat3A(&position, pTransform->GetPosition());
	position.x = m_editPositionX.GetValue();

	pTransform->SetPosition(XMLoadFloat3A(&position));
}

void CTransformInspectorFormView::OnEnChangeEditPositionY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	// 컨트롤 -> 변수로 값 업데이트
	m_editPositionY.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A position;
	XMStoreFloat3A(&position, pTransform->GetPosition());
	position.y = m_editPositionY.GetValue();

	pTransform->SetPosition(XMLoadFloat3A(&position));
}

void CTransformInspectorFormView::OnEnChangeEditPositionZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	// 컨트롤 -> 변수로 값 업데이트
	m_editPositionZ.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A position;
	XMStoreFloat3A(&position, pTransform->GetPosition());
	position.z = m_editPositionZ.GetValue();

	pTransform->SetPosition(XMLoadFloat3A(&position));
}

void CTransformInspectorFormView::OnEnChangeEditRotationX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editRotationX.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A rotation;
	XMStoreFloat3A(&rotation, ze::Math::QuaternionToEulerNormal(pTransform->GetRotation()));
	rotation.x = XMConvertToRadians(m_editRotationX.GetValue());

	pTransform->SetRotationEuler(XMLoadFloat3A(&rotation));
}

void CTransformInspectorFormView::OnEnChangeEditRotationY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editRotationY.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A rotation;
	XMStoreFloat3A(&rotation, ze::Math::QuaternionToEulerNormal(pTransform->GetRotation()));
	rotation.y = XMConvertToRadians(m_editRotationY.GetValue());

	pTransform->SetRotationEuler(XMLoadFloat3A(&rotation));
}

void CTransformInspectorFormView::OnEnChangeEditRotationZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editRotationZ.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A rotation;
	XMStoreFloat3A(&rotation, ze::Math::QuaternionToEulerNormal(pTransform->GetRotation()));
	rotation.z = XMConvertToRadians(m_editRotationZ.GetValue());

	pTransform->SetRotationEuler(XMLoadFloat3A(&rotation));
}

void CTransformInspectorFormView::OnEnChangeEditScaleX()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editScaleX.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A scale;
	XMStoreFloat3A(&scale, pTransform->GetScale());
	scale.x = m_editScaleX.GetValue();

	pTransform->SetScale(XMLoadFloat3A(&scale));
}

void CTransformInspectorFormView::OnEnChangeEditScaleY()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editScaleY.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A scale;
	XMStoreFloat3A(&scale, pTransform->GetScale());
	scale.y = m_editScaleY.GetValue();

	pTransform->SetScale(XMLoadFloat3A(&scale));
}

void CTransformInspectorFormView::OnEnChangeEditScaleZ()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	m_editScaleZ.UpdateData(TRUE);

	ze::Transform* pTransform = this->GetCLVItemToModify()->GetTransform();

	XMFLOAT3A scale;
	XMStoreFloat3A(&scale, pTransform->GetScale());
	scale.z = m_editScaleZ.GetValue();

	pTransform->SetScale(XMLoadFloat3A(&scale));
}
