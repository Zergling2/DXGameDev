#include "EditorCameraScript.h"
#include "MainFrm.h"
#include "LevelEditorView.h"
#include "TerrainInspectorFormView.h"
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\Input.h>
#include <ZergEngine\CoreSystem\Time.h>
#include <ZergEngine\CoreSystem\Math.h>

constexpr float SENSITIVITY = 0.12f;
constexpr float SPEED = 5.0f;
constexpr float WALK_SPEED = SPEED * 0.25f;
constexpr float HEAD_CLAMP_ANGLE = 89.0f;

EditorCameraScript::EditorCameraScript(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
	, m_camera()
	, m_mouseMoved(false)
	, m_prevMousePos{ 0, 0 }
{
}

void EditorCameraScript::Start()
{
	m_camera = this->GetGameObjectHandle().ToPtr()->GetComponent<ze::Camera>();
	m_camera.ToPtr()->SetClippingPlanes(0.03f, 500.0f);
}

void EditorCameraScript::Update()
{
	CMainFrame* pMainFrame = static_cast<CMainFrame*>(AfxGetMainWnd());
	if (!pMainFrame->GetLevelEditorView()->IsFocused())
		return;

	this->UpdateMouseMovedInfo();

	// 애디터 뷰 화면에 마우스 R버튼이 눌린 상태일 시
	if (ze::Input::GetInstance()->GetMouseButton(ze::MouseButton::Right))
	{
		this->MoveCamera();
		return;
	}

	if (ze::Input::GetInstance()->GetMouseButtonDown(ze::MouseButton::Left)||
		(ze::Input::GetInstance()->GetMouseButton(ze::MouseButton::Left) && this->DidMouseMoved()))
	{
		CFormView* pInspectorFormView = pMainFrame->GetInspectorFormView();

		if (pInspectorFormView->IsKindOf(RUNTIME_CLASS(CTerrainInspectorFormView)))
			this->EditTerrain(static_cast<CTerrainInspectorFormView*>(pInspectorFormView));
	}
}

void EditorCameraScript::MoveCamera()
{
	ze::GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();

	const float dt = ze::Time::GetInstance()->GetDeltaTime();

	XMVECTOR localRotation = pGameObject->m_transform.GetRotation();
	XMVECTOR worldForwardAxis = XMVector3Rotate(ze::Vector3::Forward(), localRotation);
	XMVECTOR worldRightAxis = XMVector3Rotate(ze::Vector3::Right(), localRotation);

	const int32_t mx = ze::Input::GetInstance()->GetMouseAxisHorizontal();
	const int32_t my = ze::Input::GetInstance()->GetMouseAxisVertical();
	if (mx != 0 || my != 0)
	{
		XMVECTOR temp = ze::Math::QuaternionToEuler(localRotation);
		XMFLOAT3A rotationEuler;
		XMStoreFloat3A(&rotationEuler, temp);

		rotationEuler.x += XMConvertToRadians(static_cast<float>(my) * SENSITIVITY);
		rotationEuler.x = ze::Math::Clamp(rotationEuler.x, XMConvertToRadians(-HEAD_CLAMP_ANGLE), XMConvertToRadians(+HEAD_CLAMP_ANGLE));
		rotationEuler.y += XMConvertToRadians(static_cast<float>(mx) * SENSITIVITY);
		rotationEuler.z = 0.0f;

		temp = XMLoadFloat3A(&rotationEuler);
		pGameObject->m_transform.SetRotationEuler(temp);
	}

	const float speed = dt * (ze::Input::GetInstance()->GetKey(ze::KEY_LSHIFT) ? WALK_SPEED : SPEED);

	if (ze::Input::GetInstance()->GetKey(ze::KEY_W))
		pGameObject->m_transform.Translate(worldForwardAxis * speed);

	if (ze::Input::GetInstance()->GetKey(ze::KEY_S))
		pGameObject->m_transform.Translate(worldForwardAxis * -speed);

	if (ze::Input::GetInstance()->GetKey(ze::KEY_A))
		pGameObject->m_transform.Translate(worldRightAxis * -speed);

	if (ze::Input::GetInstance()->GetKey(ze::KEY_D))
		pGameObject->m_transform.Translate(worldRightAxis * speed);
}

static void BrushTerrain(CTerrainInspectorFormView* pTerrainInspectorFormView, TERRAIN_EDIT_MODE tem, const ze::Ray& ray)
{
	TerrainDataEditor& tde = pTerrainInspectorFormView->GetTerrainDataEditor();

	switch (tem)
	{
	case TERRAIN_EDIT_MODE::RAISE_OR_LOWER_TERRAIN:
		break;
	case TERRAIN_EDIT_MODE::PAINT_TEXTURE:
		// tde.
		break;
	case TERRAIN_EDIT_MODE::SET_HEIGHT:
		break;
	case TERRAIN_EDIT_MODE::SMOOTH_HEIGHT:
		break;
	}
}

void EditorCameraScript::EditTerrain(CTerrainInspectorFormView* pTerrainInspectorFormView)
{
	POINT currMousePos = ze::Input::GetInstance()->GetMousePosition();

	ze::Camera* pCamera = m_camera.ToPtr();
	POINT pt = pCamera->ScreenPointToCameraPoint(currMousePos);
	// POINT pt = currMousePos;	// ze::Camera::ScreenPointToCameraPoint 함수는 전체화면 카메라의 경우 생략 가능하긴 함
	ze::Ray ray = pCamera->ScreenPointToRay(pt);

	const TERRAIN_BRUSH_TYPE tbt = pTerrainInspectorFormView->GetTerrainBrushType();
	const TERRAIN_EDIT_MODE tem = pTerrainInspectorFormView->GetTerrainEditMode();
	switch (tbt)
	{
	case TERRAIN_BRUSH_TYPE::BRUSH_TERRAIN:
		BrushTerrain(pTerrainInspectorFormView, tem, ray);
		break;
	case TERRAIN_BRUSH_TYPE::BRUSH_TREE:
		break;
	case TERRAIN_BRUSH_TYPE::BRUSH_GRASS:
		break;
	default:
		break;
	}
}

void EditorCameraScript::UpdateMouseMovedInfo()
{
	const POINT currMousePos = ze::Input::GetInstance()->GetMousePosition();

	if (m_prevMousePos.x != currMousePos.x || m_prevMousePos.y != currMousePos.y)
		m_mouseMoved = true;
	else
		m_mouseMoved = false;

	m_prevMousePos = currMousePos;
}
