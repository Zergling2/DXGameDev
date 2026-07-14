#include "Warehouse.h"
#include "..\Script\Player.h"
#include "..\Script\GameUIManager.h"
#include "..\Script\ThirdPersonCharacter.h"
#include "..\Script\CollisionEventTest.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(Warehouse);

static const wchar_t* PLAYER_UI_TEXT_FONT = L"Agency FB";

void Warehouse::CreateShortContainer(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"short container");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshShortContainer);
	pMeshRenderer->SetMaterial(0, m_matShortContainer);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3& rbLocalPos = m_meshShortContainer->GetAabb().Center;	// 로컬 위치
	XMFLOAT4 rbLocalRot;	// 로컬 회전
	XMStoreFloat4(&rbLocalRot, XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderShortContainer, rbLocalPos, rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateLongContainer(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"long container");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshLongContainer);
	pMeshRenderer->SetMaterial(0, m_matLongContainer);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3& rbLocalPos = m_meshLongContainer->GetAabb().Center;	// 로컬 위치
	XMFLOAT4 rbLocalRot;	// 로컬 회전
	XMStoreFloat4(&rbLocalRot, XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderLongContainer, rbLocalPos, rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateWoodenBox8060(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"wooden box");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshWoodenBox8060);
	pMeshRenderer->SetMaterial(0, m_matWoodenBox);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3& rbLocalPos = m_meshWoodenBox8060->GetAabb().Center;	// 로컬 위치
	XMFLOAT4 rbLocalRot;	// 로컬 회전
	XMStoreFloat4(&rbLocalRot, XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderWoodenBox8060, rbLocalPos, rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateWoodenBox9070(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"wooden box");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshWoodenBox9070);
	pMeshRenderer->SetMaterial(0, m_matWoodenBox);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3& rbLocalPos = m_meshWoodenBox9070->GetAabb().Center;	// 로컬 위치
	XMFLOAT4 rbLocalRot;	// 로컬 회전
	XMStoreFloat4(&rbLocalRot, XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderWoodenBox9070, rbLocalPos, rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateWoodenBox10090(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"wooden box");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshWoodenBox10090);
	pMeshRenderer->SetMaterial(0, m_matWoodenBox);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3& rbLocalPos = m_meshWoodenBox10090->GetAabb().Center;	// 로컬 위치
	XMFLOAT4 rbLocalRot;	// 로컬 회전
	XMStoreFloat4(&rbLocalRot, XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderWoodenBox10090, rbLocalPos, rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreatePaperBox(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"paper box");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshPaperBox);
	pMeshRenderer->SetMaterial(0, m_matPaperBox);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3& rbLocalPos = m_meshPaperBox->GetAabb().Center;	// 로컬 위치
	XMFLOAT4 rbLocalRot;	// 로컬 회전
	XMStoreFloat4(&rbLocalRot, XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderPaperBox, rbLocalPos, rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateOpenContainer1(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"open container1");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshOpenContainer1);
	pMeshRenderer->SetMaterial(0, m_matOpenContainer1);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3 rbLocalPos[8] =
	{
		XMFLOAT3(0.0f, 0.03f, 0.0f),
		XMFLOAT3(-1.27f, 1.3f, 0.0f),
		XMFLOAT3(0.0f, 2.57f, 0.0f),
		XMFLOAT3(1.27f, 1.3f, 0.0f),
		XMFLOAT3(-1.9159f, 1.3f, -3.3105f),
		XMFLOAT3(-1.6515f, 1.3f, 2.5531f),
		XMFLOAT3(1.6513f, 1.3f, 2.5532f),
		XMFLOAT3(0.65f, 1.3f, -3.13f)
	};
	XMFLOAT4 rbLocalRot[8];
	XMStoreFloat4(&rbLocalRot[0], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f));
	XMStoreFloat4(&rbLocalRot[1], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(-90.0f)));
	XMStoreFloat4(&rbLocalRot[2], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f));
	XMStoreFloat4(&rbLocalRot[3], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(-90.0f)));
	XMStoreFloat4(&rbLocalRot[4], XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(-16.146f), 0.0f));
	XMStoreFloat4(&rbLocalRot[5], XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(-60.033f), 0.0f));
	XMStoreFloat4(&rbLocalRot[6], XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(59.963f), 0.0f));
	XMStoreFloat4(&rbLocalRot[7], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f));

	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer1[0], rbLocalPos[0], rbLocalRot[0]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer1[0], rbLocalPos[1], rbLocalRot[1]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer1[0], rbLocalPos[2], rbLocalRot[2]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer1[0], rbLocalPos[3], rbLocalRot[3]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer1[1], rbLocalPos[4], rbLocalRot[4]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer1[1], rbLocalPos[5], rbLocalRot[5]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer1[1], rbLocalPos[6], rbLocalRot[6]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer1[1], rbLocalPos[7], rbLocalRot[7]).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateOpenContainer2(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"open container2");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshOpenContainer2);
	pMeshRenderer->SetMaterial(0, m_matOpenContainer2);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3 rbLocalPos[8] =
	{
		XMFLOAT3(0.0f, 0.03f, 0.0f),
		XMFLOAT3(-1.27f, 1.3f, 0.0f),
		XMFLOAT3(0.0f, 2.57f, 0.0f),
		XMFLOAT3(1.27f, 1.3f, 0.0f),
		XMFLOAT3(-1.9356f, 1.3f, -3.2394f),
		XMFLOAT3(-1.7224f, 1.3f, 2.6051f),
		XMFLOAT3(1.7064f, 1.3f, 2.5917f),
		XMFLOAT3(1.9337f, 1.3f, -3.2473f)
	};
	XMFLOAT4 rbLocalRot[8];
	XMStoreFloat4(&rbLocalRot[0], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f));
	XMStoreFloat4(&rbLocalRot[1], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(-90.0f)));
	XMStoreFloat4(&rbLocalRot[2], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, 0.0f));
	XMStoreFloat4(&rbLocalRot[3], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(-90.0f)));
	XMStoreFloat4(&rbLocalRot[4], XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(-9.7258f), 0.0f));
	XMStoreFloat4(&rbLocalRot[5], XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(-52.155f), 0.0f));
	XMStoreFloat4(&rbLocalRot[6], XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(53.995f), 0.0f));
	XMStoreFloat4(&rbLocalRot[7], XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(10.442f), 0.0f));

	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer2[0], rbLocalPos[0], rbLocalRot[0]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer2[0], rbLocalPos[1], rbLocalRot[1]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer2[0], rbLocalPos[2], rbLocalRot[2]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer2[0], rbLocalPos[3], rbLocalRot[3]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer2[1], rbLocalPos[4], rbLocalRot[4]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer2[1], rbLocalPos[5], rbLocalRot[5]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer2[1], rbLocalPos[6], rbLocalRot[6]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderOpenContainer2[1], rbLocalPos[7], rbLocalRot[7]).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateHouseSideWall(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"house side wall");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshHouseSideWall);
	pMeshRenderer->SetMaterial(0, m_matHouseSideWall[0]);
	pMeshRenderer->SetMaterial(1, m_matHouseSideWall[1]);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3& rbLocalPos = m_meshHouseSideWall->GetAabb().Center;	// 로컬 위치
	XMFLOAT4 rbLocalRot;	// 로컬 회전
	XMStoreFloat4(&rbLocalRot, XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderHouseSideWall, rbLocalPos, rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateRedTeamBase(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"redbase");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshHouseRedBase);
	pMeshRenderer->SetMaterial(0, m_matHouseRedBase[0]);
	pMeshRenderer->SetMaterial(1, m_matHouseRedBase[1]);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3 rbLocalPos[8] =
	{
		XMFLOAT3(0.0f, 1.5f, 9.9f),
		XMFLOAT3(-14.1f, 1.5f, -2.0f),
		XMFLOAT3(-14.1f, 4.5f, -2.0f),
		XMFLOAT3(-19.0f, 1.5f, 2.9f),
		XMFLOAT3(-19.0f, 4.5f, 2.9f),
		XMFLOAT3(-18.0f, 1.5f, 5.0f),
		XMFLOAT3(-19.0f, 1.5f, 9.9f),
		XMFLOAT3(14.1f, 1.5f, 5.0f)
	};
	XMFLOAT4 rbLocalRot[8];
	XMStoreFloat4(&rbLocalRot[0], XMQuaternionIdentity());
	XMStoreFloat4(&rbLocalRot[1], XMQuaternionIdentity());
	XMStoreFloat4(&rbLocalRot[2], XMQuaternionIdentity());
	XMStoreFloat4(&rbLocalRot[3], XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(-90.0f), 0.0f));
	XMStoreFloat4(&rbLocalRot[4], XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(-90.0f), 0.0f));
	XMStoreFloat4(&rbLocalRot[5], XMQuaternionIdentity());
	XMStoreFloat4(&rbLocalRot[6], XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(-90.0f), 0.0f));
	XMStoreFloat4(&rbLocalRot[7], XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBase[0], rbLocalPos[0], rbLocalRot[0]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBase[1], rbLocalPos[1], rbLocalRot[1]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBase[1], rbLocalPos[2], rbLocalRot[2]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBase[1], rbLocalPos[3], rbLocalRot[3]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBase[1], rbLocalPos[4], rbLocalRot[4]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBase[1], rbLocalPos[5], rbLocalRot[5]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBase[1], rbLocalPos[6], rbLocalRot[6]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBase[1], rbLocalPos[7], rbLocalRot[7]).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateRedBaseWall(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"redbasewall");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshHouseRedBaseWall);
	pMeshRenderer->SetMaterial(0, m_matHouseRedBaseWall[0]);
	pMeshRenderer->SetMaterial(1, m_matHouseRedBaseWall[1]);
	pMeshRenderer->SetMaterial(2, m_matHouseRedBaseWall[2]);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3 rbLocalPos[6] =
	{
		XMFLOAT3(0.0f, 6.0f, 0.0f),
		XMFLOAT3(-13.4f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, -0.0f),
		XMFLOAT3(+13.4f, 1.0f, 0.0f),
		XMFLOAT3(-6.7f, 2.85f, 0.65f),
		XMFLOAT3(+6.7f, 2.85f, 0.65f),
	};
	// XMFLOAT4 rbLocalRot[6];
	// XMStoreFloat4(&rbLocalRot[0], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[1], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[2], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[3], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[4], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[5], XMQuaternionIdentity());
	XMFLOAT4 rbLocalRot;
	XMStoreFloat4(&rbLocalRot, XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBaseWall[0], rbLocalPos[0], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBaseWall[1], rbLocalPos[1], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBaseWall[1], rbLocalPos[2], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBaseWall[1], rbLocalPos[3], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBaseWall[2], rbLocalPos[4], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRedBaseWall[2], rbLocalPos[5], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateBlueTeamBase(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"bluebase");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshHouseBlueBase);
	pMeshRenderer->SetMaterial(0, m_matHouseBlueBase[0]);
	pMeshRenderer->SetMaterial(1, m_matHouseBlueBase[1]);
	pMeshRenderer->SetMaterial(2, m_matHouseBlueBase[2]);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3 rbLocalPos[10] =
	{
		XMFLOAT3(0.0f, 3.1f, -5.0f),
		XMFLOAT3(14.1f, 1.5f, -5.0f),
		XMFLOAT3(-14.1f, 1.5f, -5.0f),
		XMFLOAT3(0.0f, 1.5f, -10.1f),
		XMFLOAT3(0.0f, 0.5f, -2.15f),
		XMFLOAT3(0.0f, 1.45f, -4.1f),
		XMFLOAT3(0.0f, 1.45f, -0.2f),
		XMFLOAT3(0.0f, 2.8f, -2.15f),
		XMFLOAT3(0.0f, 1.95f, -2.15f),
		XMFLOAT3(0.0f, 1.45f, -3.25f)
	};
	// XMFLOAT4 rbLocalRot[10];
	// XMStoreFloat4(&rbLocalRot[0], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[1], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[2], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[3], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[4], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[5], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[6], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[7], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[8], XMQuaternionIdentity());
	// XMStoreFloat4(&rbLocalRot[9], XMQuaternionIdentity());
	XMFLOAT4 rbLocalRot;
	XMStoreFloat4(&rbLocalRot, XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBase[0], rbLocalPos[0], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBase[1], rbLocalPos[1], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBase[1], rbLocalPos[2], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBase[2], rbLocalPos[3], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBase[3], rbLocalPos[4], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBase[4], rbLocalPos[5], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBase[4], rbLocalPos[6], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBase[5], rbLocalPos[7], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBase[6], rbLocalPos[8], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBase[7], rbLocalPos[9], rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateBlueBaseWall(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"bluebasewall");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshHouseBlueBaseWall);
	pMeshRenderer->SetMaterial(0, m_matHouseBlueBaseWall[0]);
	pMeshRenderer->SetMaterial(1, m_matHouseBlueBaseWall[1]);
	pMeshRenderer->SetMaterial(2, m_matHouseBlueBaseWall[2]);
	pMeshRenderer->SetMaterial(3, m_matHouseBlueBaseWall[3]);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3 rbLocalPos[4] =
	{
		XMFLOAT3(0.0f, 6.0f, 0.0f),
		XMFLOAT3(-13.4f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, -0.0f),
		XMFLOAT3(+13.4f, 1.0f, 0.0f)
	};
	XMFLOAT4 rbLocalRot[4];
	XMStoreFloat4(&rbLocalRot[0], XMQuaternionIdentity());
	XMStoreFloat4(&rbLocalRot[1], XMQuaternionIdentity());
	XMStoreFloat4(&rbLocalRot[2], XMQuaternionIdentity());
	XMStoreFloat4(&rbLocalRot[3], XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBaseWall[0], rbLocalPos[0], rbLocalRot[0]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBaseWall[1], rbLocalPos[1], rbLocalRot[1]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBaseWall[1], rbLocalPos[2], rbLocalRot[2]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseBlueBaseWall[1], rbLocalPos[3], rbLocalRot[3]).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateHouseRoof(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"houseroof");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshHouseRoof);
	pMeshRenderer->SetMaterial(0, m_matHouseRoof);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3 rbLocalPos[2] =
	{
		XMFLOAT3(-7.6746f, 2.1692f, 0.0f),
		XMFLOAT3(+7.6746f, 2.1692f, 0.0f)
	};
	XMFLOAT4 rbLocalRot[2];
	XMStoreFloat4(&rbLocalRot[0], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(15.7f)));
	XMStoreFloat4(&rbLocalRot[1], XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(-15.7f)));

	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRoof, rbLocalPos[0], rbLocalRot[0]).ToPtr()->SetBodyType(RigidbodyType::Static);
	pGameObject->AddComponent<Rigidbody>(m_colliderHouseRoof, rbLocalPos[1], rbLocalRot[1]).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::CreateBarrel1(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	CreateBarrelImpl(0, pos, rot);
}

void Warehouse::CreateBarrel2(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	CreateBarrelImpl(1, pos, rot);
}

void Warehouse::CreateBarrel3(const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	CreateBarrelImpl(2, pos, rot);
}

void Warehouse::CreateBarrelImpl(size_t matIndex, const XMFLOAT3& pos, const XMFLOAT3& rot)
{
	GameObjectHandle hGameObject = CreateGameObject(L"houseroof");
	GameObject* pGameObject = hGameObject.ToPtr();
	pGameObject->m_transform.SetPosition(pos);
	pGameObject->m_transform.SetRotationEuler(rot);

	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	pMeshRenderer->SetMesh(m_meshBarrel);
	pMeshRenderer->SetMaterial(0, m_matBarrel[matIndex]);

	// 콜라이더 로컬 오프셋
	const XMFLOAT3& rbLocalPos = m_meshBarrel->GetAabb().Center;	// 로컬 위치
	XMFLOAT4 rbLocalRot;	// 로컬 회전
	XMStoreFloat4(&rbLocalRot, XMQuaternionIdentity());

	pGameObject->AddComponent<Rigidbody>(m_colliderBarrel, rbLocalPos, rbLocalRot).ToPtr()->SetBodyType(RigidbodyType::Static);
}

void Warehouse::OnLoadScene()
{
	{
		GameObjectHandle hGameObjectTest = CreateGameObject();
		GameObject* pGameObjectTest = hGameObjectTest.ToPtr();

		pGameObjectTest->m_transform.SetPosition(0, 3, 0);

		pGameObjectTest->AddComponent<Rigidbody>(std::make_shared<CapsuleCollider>(0.2f, 1.0f));
	}
	



	// ## Lights
	{
		GameObjectHandle hSun = CreateGameObject(L"house light1");
		GameObject* pSun = hSun.ToPtr();
		pSun->m_transform.SetRotationEuler(XMConvertToRadians(50), XMConvertToRadians(45), 0.0f);
		ComponentHandle<DirectionalLight> hLight = pSun->AddComponent<DirectionalLight>();
		DirectionalLight* pLight = hLight.ToPtr();
		XMStoreFloat4A(&pLight->m_diffuse, XMVectorScale(Vector3::One(), 0.15f));
		XMStoreFloat4A(&pLight->m_specular, Vector3::OneHalf());
	}

	{
		GameObjectHandle hSun = CreateGameObject(L"house light2");
		GameObject* pSun = hSun.ToPtr();
		pSun->m_transform.SetRotationEuler(XMConvertToRadians(50), XMConvertToRadians(-135), 0.0f);
		ComponentHandle<DirectionalLight> hLight = pSun->AddComponent<DirectionalLight>();
		DirectionalLight* pLight = hLight.ToPtr();
		XMStoreFloat4A(&pLight->m_diffuse, XMVectorScale(Vector3::One(), 0.15f));
		XMStoreFloat4A(&pLight->m_specular, Vector3::OneHalf());
	}

	// ## Player
	ComponentHandle<Player> hScriptPlayer;
	{
		GameObjectHandle hGameObjectPlayer = CreateGameObject(L"Player");
		GameObject* pGameObjectPlayer = hGameObjectPlayer.ToPtr();
		pGameObjectPlayer->m_transform.SetPosition(-7.0f, 4.0f, -5.0f);

		hScriptPlayer = pGameObjectPlayer->AddComponent<Player>();		// 플레이어 스크립트
	}
	Player* pScriptPlayer = hScriptPlayer.ToPtr();



	GameObjectHandle hGameObjectThirdPersonCharacter = CreateGameObject(L"TPC");
	GameObject* pGameObjectThirdPersonCharacter = hGameObjectThirdPersonCharacter.ToPtr();
	pGameObjectThirdPersonCharacter->m_transform.SetPosition(1, 0, 1);
	pGameObjectThirdPersonCharacter->AddComponent<ThirdPersonCharacter>();


	
	// Adapter Info UI
	{
		UIObjectHandle hText = CreateText();
		Text* pText = static_cast<Text*>(hText.ToPtr());
		pText->SetSize(XMFLOAT2(256, 16));
		std::wstring text = GraphicDevice::GetInstance()->GetAdapterDescription();
		pText->SetText(std::move(text));
		pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pText->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
		pText->m_transform.SetPosition(128 + 2, -(8));
		pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pText->SetColor(ColorsLinear::Orange);
		pText->GetTextFormat().SetSize(12);
		pText->GetTextFormat().SetFontFamilyName(L"Consolas");
		pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pText->ApplyTextFormat();
	}

	{
		UIObjectHandle hText = CreateText();
		Text* pText = static_cast<Text*>(hText.ToPtr());
		pText->SetSize(XMFLOAT2(256, 16));
		std::wstring text = L"DedicatedVideoMemory: ";
		size_t val = GraphicDevice::GetInstance()->GetAdapterDedicatedVideoMemory();
		text += std::to_wstring(val / (1024 * 1024));
		text += L"MB";
		pText->SetText(std::move(text));
		pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pText->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
		pText->m_transform.SetPosition(128 + 2, -(8 + 16 * 1));
		pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pText->SetColor(ColorsLinear::Orange);
		pText->GetTextFormat().SetSize(12);
		pText->GetTextFormat().SetFontFamilyName(L"Consolas");
		pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pText->ApplyTextFormat();
	}

	{
		UIObjectHandle hText = CreateText();
		Text* pText = static_cast<Text*>(hText.ToPtr());
		pText->SetSize(XMFLOAT2(256, 16));
		std::wstring text = L"DedicatedSystemMemory: ";
		size_t val = GraphicDevice::GetInstance()->GetAdapterDedicatedSystemMemory();
		text += std::to_wstring(val / (1024 * 1024));
		text += L"MB";
		pText->SetText(std::move(text));
		pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pText->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
		pText->m_transform.SetPosition(128 + 2, -(8 + 16 * 2));
		pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pText->SetColor(ColorsLinear::Orange);
		pText->GetTextFormat().SetSize(12);
		pText->GetTextFormat().SetFontFamilyName(L"Consolas");
		pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pText->ApplyTextFormat();
	}

	{
		UIObjectHandle hText = CreateText();
		Text* pText = static_cast<Text*>(hText.ToPtr());
		pText->SetSize(XMFLOAT2(256, 16));
		std::wstring text = L"SharedSystemMemory: ";
		size_t val = GraphicDevice::GetInstance()->GetAdapterSharedSystemMemory();
		text += std::to_wstring(val / (1024 * 1024));
		text += L"MB";
		pText->SetText(std::move(text));
		pText->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pText->m_transform.SetVerticalAnchor(VerticalAnchor::Top);
		pText->m_transform.SetPosition(128 + 2, -(8 + 16 * 3));
		pText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		pText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pText->SetColor(ColorsLinear::Orange);
		pText->GetTextFormat().SetSize(12);
		pText->GetTextFormat().SetFontFamilyName(L"Consolas");
		pText->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pText->ApplyTextFormat();
	}

	GameObjectHandle hGameObjectGameUIManager = CreateGameObject();
	GameObject* pGameObjectGameUIManager = hGameObjectGameUIManager.ToPtr();
	ComponentHandle<GameUIManager> hScriptGameUIManager = pGameObjectGameUIManager->AddComponent<GameUIManager>();
	pScriptPlayer->m_hScriptGameUIManager = hScriptGameUIManager;
	GameUIManager* pScriptGameUIManager = hScriptGameUIManager.ToPtr();
	pScriptGameUIManager->m_hScriptPlayer = hScriptPlayer;

	// ## UI
	{
		UIObjectHandle hImageCrosshair = CreateImage();
		pScriptGameUIManager->m_hImageCrosshair = hImageCrosshair;
		Image* pImageCrosshair = static_cast<Image*>(hImageCrosshair.ToPtr());
		pImageCrosshair->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\crosshair.dds"));
		pImageCrosshair->SetNativeSize(true);
		pImageCrosshair->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pImageCrosshair->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);

		UIObjectHandle hImageHealthBackground = CreateImage();
		pScriptGameUIManager->m_hImageHealthBackground = hImageHealthBackground;
		Image* pImageHealthBackground = static_cast<Image*>(hImageHealthBackground.ToPtr());
		pImageHealthBackground->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\health.png"));
		pImageHealthBackground->SetNativeSize(true);
		pImageHealthBackground->m_transform.SetPosition(pImageHealthBackground->GetHalfSizeX() + 4, pImageHealthBackground->GetHalfSizeY() + 4);
		pImageHealthBackground->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pImageHealthBackground->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);

		UIObjectHandle hImageRBUIBackground = CreateImage();
		pScriptGameUIManager->m_hImageRBUIBackground = hImageRBUIBackground;
		Image* pImageRBUIBackground = static_cast<Image*>(hImageRBUIBackground.ToPtr());
		pImageRBUIBackground->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapon_indicator.png"));
		pImageRBUIBackground->SetNativeSize(true);
		pImageRBUIBackground->m_transform.SetPosition(-pImageRBUIBackground->GetHalfSizeX() - 4, pImageRBUIBackground->GetHalfSizeY() + 4);
		pImageRBUIBackground->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
		pImageRBUIBackground->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);

		UIObjectHandle hTextHP = CreateText();
		pScriptGameUIManager->m_hTextHP = hTextHP;
		Text* pTextHP = static_cast<Text*>(hTextHP.ToPtr());
		pTextHP->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pTextHP->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextHP->m_transform.SetPosition(pImageHealthBackground->m_transform.GetPosition());
		pTextHP->m_transform.TranslateX(-56);
		pTextHP->SetSize(128, 48);
		pTextHP->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextHP->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextHP->SetColor(ColorsLinear::White);
		pTextHP->GetTextFormat().SetSize(28);
		pTextHP->GetTextFormat().SetFontFamilyName(L"Impact");
		pTextHP->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pTextHP->ApplyTextFormat();

		UIObjectHandle hTextAP = CreateText();
		pScriptGameUIManager->m_hTextAP = hTextAP;
		Text* pTextAP = static_cast<Text*>(hTextAP.ToPtr());
		pTextAP->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pTextAP->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextAP->m_transform.SetPosition(pImageHealthBackground->m_transform.GetPosition());
		pTextAP->m_transform.TranslateX(120);
		pTextAP->SetSize(128, 48);
		pTextAP->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextAP->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextAP->SetColor(ColorsLinear::White);
		pTextAP->GetTextFormat().SetSize(28);
		pTextAP->GetTextFormat().SetFontFamilyName(L"Impact");
		pTextAP->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_MEDIUM);
		pTextAP->ApplyTextFormat();

		UIObjectHandle hTextPoint = CreateText();
		pScriptGameUIManager->m_hTextPoint = hTextPoint;
		Text* pTextPoint = static_cast<Text*>(hTextPoint.ToPtr());
		pTextPoint->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
		pTextPoint->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextPoint->m_transform.SetPosition(pImageRBUIBackground->m_transform.GetPosition());
		pTextPoint->m_transform.Translate(-45, 34);
		pTextPoint->SetSize(128, 32);
		pTextPoint->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextPoint->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextPoint->SetColor(ColorsLinear::White);
		pTextPoint->GetTextFormat().SetSize(22);
		pTextPoint->GetTextFormat().SetFontFamilyName(PLAYER_UI_TEXT_FONT);
		pTextPoint->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
		pTextPoint->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
		pTextPoint->ApplyTextFormat();

		UIObjectHandle hTextWeaponName = CreateText();
		pScriptGameUIManager->m_hTextWeaponName = hTextWeaponName;
		Text* pTextWeaponName = static_cast<Text*>(hTextWeaponName.ToPtr());
		pTextWeaponName->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
		pTextWeaponName->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextWeaponName->m_transform.SetPosition(pImageRBUIBackground->m_transform.GetPosition());
		pTextWeaponName->m_transform.TranslateY(6);
		pTextWeaponName->SetSize(XMFLOAT2(200, 32));
		pTextWeaponName->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextWeaponName->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextWeaponName->SetColor(ColorsLinear::White);
		pTextWeaponName->GetTextFormat().SetSize(22);
		pTextWeaponName->GetTextFormat().SetFontFamilyName(PLAYER_UI_TEXT_FONT);
		pTextWeaponName->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
		pTextWeaponName->ApplyTextFormat();

		UIObjectHandle hTextAmmoState = CreateText();
		pScriptGameUIManager->m_hTextAmmoState = hTextAmmoState;
		Text* pTextAmmoState = static_cast<Text*>(hTextAmmoState.ToPtr());
		pTextAmmoState->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
		pTextAmmoState->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextAmmoState->m_transform.SetPosition(pImageRBUIBackground->m_transform.GetPosition());
		pTextAmmoState->m_transform.Translate(16, -28);
		pTextAmmoState->SetSize(XMFLOAT2(128, 40));
		pTextAmmoState->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextAmmoState->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextAmmoState->SetColor(ColorsLinear::White);
		pTextAmmoState->GetTextFormat().SetSize(34);
		pTextAmmoState->GetTextFormat().SetFontFamilyName(PLAYER_UI_TEXT_FONT);
		pTextAmmoState->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
		pTextAmmoState->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
		pTextAmmoState->ApplyTextFormat();
	}

	// ## MATERIALS
	// std::shared_ptr<Material> matAsphault = ResourceLoader::GetInstance()->CreateMaterial();
	// XMStoreFloat4A(&matAsphault->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.8f), 1.0f));
	// XMStoreFloat4A(&matAsphault->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	// matAsphault->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\asphault\\AsphaultStreet_d.tga");
	// matAsphault->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\asphault\\AsphaultStreet_n.tga");

	std::shared_ptr<Material> matBambooWoodSemigloss = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matBambooWoodSemigloss->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.4f), 1.0f));
	XMStoreFloat4A(&matBambooWoodSemigloss->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matBambooWoodSemigloss->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\wood\\bamboo-wood-semigloss-diffuse.png");
	matBambooWoodSemigloss->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\wood\\bamboo-wood-semigloss-normal.png");

	std::shared_ptr<Material> matBrick22 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matBrick22->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.75f), 1.0f));
	XMStoreFloat4A(&matBrick22->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matBrick22->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\Brick 22 - 256x256.png");

	std::shared_ptr<Material> matRoof24 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matRoof24->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.75f), 1.0f));
	XMStoreFloat4A(&matRoof24->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matRoof24->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\roof\\Roof 24 - 256x256.png");

	std::shared_ptr<Material> matHouseFrame = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matHouseFrame->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::Brown, 0.4f), 1.0f));
	XMStoreFloat4A(&matHouseFrame->m_specular, XMVectorScale(ColorsLinear::White, 0.25f));
	matHouseFrame->m_specular.w = 8.0f;

	// std::shared_ptr<Material> matRustedSteelHotspot = ResourceLoader::GetInstance()->CreateMaterial();
	// XMStoreFloat4A(&matRustedSteelHotspot->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.75f), 1.0f));
	// XMStoreFloat4A(&matRustedSteelHotspot->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.5f), 1.0f));
	// matRustedSteelHotspot->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metals\\RustedSteel\\RustedSteel_Diffuse.png");
	// matRustedSteelHotspot->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metals\\RustedSteel\\RustedSteel_Normal.png");

	std::shared_ptr<Material> matConcrete3 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matConcrete3->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
	XMStoreFloat4A(&matConcrete3->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	matConcrete3->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\concrete\\concrete3_diffuse.png");
	matConcrete3->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\concrete\\concrete3_normal.png");

	std::shared_ptr<Material> matConcrete2 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matConcrete2->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
	XMStoreFloat4A(&matConcrete2->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	matConcrete2->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\concrete\\concrete2_diffuse.png");

	std::shared_ptr<Material> matSprayedWall1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matSprayedWall1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.5f), 1.0f));
	XMStoreFloat4A(&matSprayedWall1->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	matSprayedWall1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\wall\\sprayed-wall1_diffuse.png");
	matSprayedWall1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\wall\\sprayed-wall1_normal.png");

	std::shared_ptr<Material> matVentedMetalPanel1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matVentedMetalPanel1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.7f), 1.0f));
	XMStoreFloat4A(&matVentedMetalPanel1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.5f), 4.0f));
	matVentedMetalPanel1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metal\\vented-metal-panel1_diffuse.png");
	matVentedMetalPanel1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metal\\vented-metal-panel1_normal.png");

	// std::shared_ptr<Material> matMetalVentilation1 = ResourceLoader::GetInstance()->CreateMaterial();
	// XMStoreFloat4A(&matMetalVentilation1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.4f), 1.0f));
	// XMStoreFloat4A(&matMetalVentilation1->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.5f), 1.0f));
	// matMetalVentilation1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metal\\metal-ventilation1-diffuse.png");
	// matMetalVentilation1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\metal\\metal-ventilation1-normal.png");

	std::shared_ptr<Material> matNarrowbrick1 = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matNarrowbrick1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.8f), 1.0f));
	XMStoreFloat4A(&matNarrowbrick1->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	matNarrowbrick1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\narrowbrick1_diffuse.png");
	matNarrowbrick1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\narrowbrick1_normal.png");

	std::shared_ptr<Material> matRedbricks2b = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matRedbricks2b->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.8f), 1.0f));
	XMStoreFloat4A(&matRedbricks2b->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
	matRedbricks2b->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\redbricks2b_diffuse.png");
	matRedbricks2b->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\redbricks2b_normal.png");

	// std::shared_ptr<Material> matModernBrick1 = ResourceLoader::GetInstance()->CreateMaterial();
	// XMStoreFloat4A(&matModernBrick1->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	// XMStoreFloat4A(&matModernBrick1->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	// matModernBrick1->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\modern-brick1_diffuse.png");
	// matModernBrick1->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\bricks\\modern-brick1_normal.png");

	std::shared_ptr<Material> matStoneTile4b = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matStoneTile4b->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.7f), 1.0f));
	XMStoreFloat4A(&matStoneTile4b->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matStoneTile4b->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\floor\\stone-tile4b_diffuse.png");
	matStoneTile4b->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\floor\\stone-tile4b_normal.png");

	std::shared_ptr<Material> matDoorFrame = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matDoorFrame->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::DarkOliveGreen, 0.75f), 1.0f));
	XMStoreFloat4A(&matDoorFrame->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));

	std::shared_ptr<Material> matSolidStone = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matDoorFrame->m_diffuse, ColorsLinear::DimGray);
	XMStoreFloat4A(&matDoorFrame->m_specular, XMVectorSetW(ColorsLinear::DarkGray, 1.0f));



	m_meshShortContainer = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\container\\short_container.obj").m_staticMeshes[0];
	m_matShortContainer = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&m_matShortContainer->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.7f), 1.0f));
	XMStoreFloat4A(&m_matShortContainer->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
	m_matShortContainer->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\container\\diffuse.png");
	m_colliderShortContainer = std::make_shared<BoxCollider>(m_meshShortContainer->GetAabb().Extents);


	m_meshLongContainer = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\container\\long_container.obj").m_staticMeshes[0];
	m_matLongContainer = m_matShortContainer;
	m_colliderLongContainer = std::make_shared<BoxCollider>(m_meshLongContainer->GetAabb().Extents);


	m_meshOpenContainer1 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\container\\open_container.obj").m_staticMeshes[0];
	m_matOpenContainer1 = m_matShortContainer;
	m_colliderOpenContainer1[0] = std::make_shared<BoxCollider>(XMFLOAT3(1.3f, 0.03f, 3.1f));
	m_colliderOpenContainer1[1] = std::make_shared<BoxCollider>(XMFLOAT3(0.65f, 1.3f, 0.03f));


	m_meshOpenContainer2 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\container\\open_container2.obj").m_staticMeshes[0];
	m_matOpenContainer2 = m_matShortContainer;
	m_colliderOpenContainer2[0] = m_colliderOpenContainer1[0];
	m_colliderOpenContainer2[1] = m_colliderOpenContainer1[1];

	m_meshWoodenBox8060 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\box\\box_80_60.obj").m_staticMeshes[0];
	m_colliderWoodenBox8060 = std::make_shared<BoxCollider>(m_meshWoodenBox8060->GetAabb().Extents);

	m_meshWoodenBox9070 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\box\\box_90_70.obj").m_staticMeshes[0];
	m_colliderWoodenBox9070 = std::make_shared<BoxCollider>(m_meshWoodenBox9070->GetAabb().Extents);

	m_meshWoodenBox10090 = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\box\\box_100_90.obj").m_staticMeshes[0];
	m_colliderWoodenBox10090 = std::make_shared<BoxCollider>(m_meshWoodenBox10090->GetAabb().Extents);

	m_matWoodenBox = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&m_matWoodenBox->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
	XMStoreFloat4A(&m_matWoodenBox->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	m_matWoodenBox->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\box\\wooden_box_diffuse.png");


	m_meshPaperBox = m_meshWoodenBox10090;
	m_matPaperBox = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&m_matPaperBox->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.7f), 1.0f));
	XMStoreFloat4A(&m_matPaperBox->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	m_matPaperBox->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\box\\paper_box_diffuse.png");
	m_colliderPaperBox = std::make_shared<BoxCollider>(m_meshPaperBox->GetAabb().Extents);

	m_meshHouseSideWall = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\house_sidewall.obj").m_staticMeshes[0];
	m_matHouseSideWall[0] = matNarrowbrick1;
	m_matHouseSideWall[1] = matVentedMetalPanel1;
	m_colliderHouseSideWall = std::make_shared<BoxCollider>(m_meshHouseSideWall->GetAabb().Extents);


	m_meshHouseBlueBase = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\house_bluebase.obj").m_staticMeshes[0];
	m_matHouseBlueBase[0] = matStoneTile4b;
	m_matHouseBlueBase[1] = matConcrete2;
	m_matHouseBlueBase[2] = matBambooWoodSemigloss;
	m_colliderHouseBlueBase[0] = std::make_shared<BoxCollider>(XMFLOAT3(14.0f, 0.1f, 5.0f));
	m_colliderHouseBlueBase[1] = std::make_shared<BoxCollider>(XMFLOAT3(0.1f, 1.5f, 5.0f));
	m_colliderHouseBlueBase[2] = std::make_shared<BoxCollider>(XMFLOAT3(14.0f, 1.5f, 0.1f));
	m_colliderHouseBlueBase[3] = std::make_shared<BoxCollider>(XMFLOAT3(0.1f, 0.5f, 1.85f));
	m_colliderHouseBlueBase[4] = std::make_shared<BoxCollider>(XMFLOAT3(0.1f, 1.45f, 0.1f));
	m_colliderHouseBlueBase[5] = std::make_shared<BoxCollider>(XMFLOAT3(0.1f, 0.1f, 1.85f));
	m_colliderHouseBlueBase[6] = std::make_shared<BoxCollider>(XMFLOAT3(0.05f, 0.05f, 1.85f));
	m_colliderHouseBlueBase[7] = std::make_shared<BoxCollider>(XMFLOAT3(0.05f, 0.45f, 0.05f));


	m_meshHouseBlueBaseWall = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\house_bluebasewall.obj").m_staticMeshes[0];
	m_matHouseBlueBaseWall[0] = matRedbricks2b;
	m_matHouseBlueBaseWall[1] = matSprayedWall1;
	m_matHouseBlueBaseWall[2] = matBambooWoodSemigloss;
	m_matHouseBlueBaseWall[3] = matDoorFrame;
	m_colliderHouseBlueBaseWall[0] = std::make_shared<BoxCollider>(XMFLOAT3(14.2f, 4.0f, 0.1f));
	m_colliderHouseBlueBaseWall[1] = std::make_shared<BoxCollider>(XMFLOAT3(6.1f, 1.0f, 0.1f));


	m_meshHouseRedBase = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\house_redbase.obj").m_staticMeshes[0];
	m_matHouseRedBase[0] = matBrick22;
	m_matHouseRedBase[1] = matConcrete3;
	m_colliderHouseRedBase[0] = m_colliderHouseBlueBase[2];
	m_colliderHouseRedBase[1] = m_colliderHouseBlueBase[1];

	m_meshHouseRedBaseWall = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\house_redbasewall.obj").m_staticMeshes[0];
	m_matHouseRedBaseWall[0] = matRedbricks2b;
	m_matHouseRedBaseWall[1] = matSolidStone;
	m_matHouseRedBaseWall[2] = matDoorFrame;
	m_colliderHouseRedBaseWall[0] = m_colliderHouseBlueBaseWall[0];
	m_colliderHouseRedBaseWall[1] = m_colliderHouseBlueBaseWall[1];
	m_colliderHouseRedBaseWall[2] = std::make_shared<BoxCollider>(XMFLOAT3(1.0f, 0.15f, 0.55f));


	m_meshHouseRoof = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\house_roof.obj").m_staticMeshes[0];
	m_matHouseRoof = matRoof24;
	m_colliderHouseRoof = std::make_shared<BoxCollider>(XMFLOAT3(8.0f, 0.1f, 26.0f));

	m_meshBarrel = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\props\\barrel\\barrel.obj").m_staticMeshes[0];
	m_matBarrel[0] = ResourceLoader::GetInstance()->CreateMaterial();
	m_matBarrel[0]->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\barrel\\textures\\drum1_base_color.png");
	m_matBarrel[0]->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\barrel\\textures\\drum1_normal.png");
	XMStoreFloat4A(&m_matBarrel[0]->m_diffuse, Vector3::One3());
	XMStoreFloat4A(&m_matBarrel[0]->m_specular, XMVectorSetW(Colors::Gray, 4.0f));
	m_matBarrel[1] = ResourceLoader::GetInstance()->CreateMaterial();
	m_matBarrel[1]->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\barrel\\textures\\drum2_base_color.png");
	m_matBarrel[1]->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\barrel\\textures\\drum2_normal.png");
	XMStoreFloat4A(&m_matBarrel[1]->m_diffuse, Vector3::One3());
	XMStoreFloat4A(&m_matBarrel[1]->m_specular, XMVectorSetW(Colors::Gray, 4.0f));
	m_matBarrel[2] = ResourceLoader::GetInstance()->CreateMaterial();
	m_matBarrel[2]->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\barrel\\textures\\drum3_base_color.png");
	m_matBarrel[2]->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\props\\barrel\\textures\\drum3_normal.png");
	XMStoreFloat4A(&m_matBarrel[2]->m_diffuse, Vector3::One3());
	XMStoreFloat4A(&m_matBarrel[2]->m_specular, XMVectorSetW(Colors::Gray, 4.0f));
	m_colliderBarrel = std::make_shared<CylinderCollider>(m_meshBarrel->GetAabb().Extents.x, m_meshBarrel->GetAabb().Extents.y);

	std::shared_ptr<StaticMesh> meshHouseFrame = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\house_frame.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseFloor = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\house_floor.obj").m_staticMeshes[0];



	// Animated Weapons(m16a1)
	// {
	// 	GameObjectHandle hGameObject = CreateGameObject(L"M16A1");
	// 	GameObject* pGameObject = hGameObject.ToPtr();
	// 	pGameObject->m_transform.SetParent(&pGameObjectPlayer->m_transform);
	// 	
	// 	pGameObject->m_transform.SetPosition(M16_PV_POS);
	// 	
	// 	ComponentHandle<SkinnedMeshRenderer> hMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
	// 	SkinnedMeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	// 	
	// 	// 재질 설정
	// 	
	// 	pMeshRenderer->SetMesh(mesh);
	// 	pMeshRenderer->SetMaterial(0, matM16A1Receiver);
	// 	pMeshRenderer->SetMaterial(1, matM16A1Furniture);
	// 	pMeshRenderer->SetMaterial(2, matSTANAG30Rds);
	// 	
	// 	
	// 	pMeshRenderer->SetArmature(armaM16);
	// 	// pMeshRenderer->PlayAnimation("m16a1_idle", true);
	// 	pMeshRenderer->PlayAnimation("m16a1_reload", true);
	// 	// pMeshRenderer->PlayAnimation("m16a1_shoot", true);
	// 	// pMeshRenderer->PlayAnimation("m16a1_run", true);
	// }
	// 
	// // Animated Weapons(m4a1)
	// {
	// 	ModelData md = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m4a1\\m4a1_pv.glb");
	// 	std::shared_ptr<SkinnedMesh> mesh = md.m_skinnedMeshes[0];
	// 	// std::shared_ptr<Armature> arma = md.m_armatures[0];	// M16 뼈대 공유
	// 	
	// 	GameObjectHandle hGameObject = CreateGameObject(L"M4A1");
	// 	GameObject* pGameObject = hGameObject.ToPtr();
	// 	pGameObject->m_transform.SetParent(&pMainCamera->m_transform);
	// 	
	// 	pGameObject->m_transform.SetPosition(M16_PV_POS);
	// 	
	// 	ComponentHandle<SkinnedMeshRenderer> hMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
	// 	SkinnedMeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	// 	
	// 	// 재질 설정
	// 	
	// 	pMeshRenderer->SetMesh(mesh);
	// 	pMeshRenderer->SetMaterial(0, matM4A1Receiver);
	// 	pMeshRenderer->SetMaterial(1, matM4A1Furniture);
	// 	pMeshRenderer->SetMaterial(2, matRearSight);
	// 	pMeshRenderer->SetMaterial(3, matSTANAG30Rds);
	// 	
	// 	pMeshRenderer->SetArmature(armaM16);
	// 	// pMeshRenderer->PlayAnimation("m16a1_idle", true);
	// 	pMeshRenderer->PlayAnimation("m16a1_reload", true);
	// 	// pMeshRenderer->PlayAnimation("m16a1_shoot", true);
	// 	// pMeshRenderer->PlayAnimation("m16a1_run", true);
	// }
	// 
	// 
	// // Animated Weapons(usp)
	// constexpr XMFLOAT3 USP_PV_OFFSET(0.04f, -0.01f, 0.41f);
	// constexpr XMFLOAT3 USP_PV_POS(FPSARM_POS.x + USP_PV_OFFSET.x, FPSARM_POS.y + USP_PV_OFFSET.y, FPSARM_POS.z + USP_PV_OFFSET.z);
	// {
	// 	std::shared_ptr<SkinnedMesh> mesh_usp = md_usp_pv.m_skinnedMeshes[0];
	// 
	// 	GameObjectHandle hGameObjectSecondaryWeaponPV = CreateGameObject(L"C.USP");
	// 	GameObject* pGameObjectSecondaryWeaponPV = hGameObjectSecondaryWeaponPV.ToPtr();
	// 	pGameObjectSecondaryWeaponPV->m_transform.SetParent(&pGameObjectPlayer->m_transform);
	// 	pGameObjectSecondaryWeaponPV->m_transform.SetPosition(USP_PV_POS);
	// 
	// 	ComponentHandle<SkinnedMeshRenderer> hMeshRendererSecondaryWeaponPV = pGameObjectSecondaryWeaponPV->AddComponent<SkinnedMeshRenderer>();
	// 	SkinnedMeshRenderer* pMeshRendererSecondaryWeaponPV = hMeshRendererSecondaryWeaponPV.ToPtr();
	// 
	// 	// 재질 설정
	// 
	// 	pMeshRendererSecondaryWeaponPV->SetMesh(mesh_usp);
	// 	pMeshRendererSecondaryWeaponPV->SetMaterial(0, mat_usp);
	// 
	// 	pMeshRendererSecondaryWeaponPV->SetArmature(arma_usp);
	// 	// pMeshRendererSecondaryWeaponPV->PlayAnimation("usp_idle", true);
	// 	pMeshRendererSecondaryWeaponPV->PlayAnimation("usp_reload", true);
	// 	// pMeshRendererSecondaryWeaponPV->PlayAnimation("usp_shoot", true);
	// 	// pMeshRendererSecondaryWeaponPV->PlayAnimation("usp_run", true);
	// }
	// 
	// // Animated Weapons(b92fs black)
	// {
	// 	ModelData md = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\b92fsb\\b92fsb_pv.glb");
	// 	std::shared_ptr<SkinnedMesh> meshB92fsB = md.m_skinnedMeshes[0];
	// 	
	// 	GameObjectHandle hGameObject = CreateGameObject(L"B.92Fs black");
	// 	GameObject* pGameObject = hGameObject.ToPtr();
	// 	pGameObject->m_transform.SetParent(&pGameObjectPlayer->m_transform);
	// 	
	// 	pGameObject->m_transform.SetPosition(USP_PV_POS);
	// 	
	// 	ComponentHandle<SkinnedMeshRenderer> hMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
	// 	SkinnedMeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	// 	
	// 	// 재질 설정
	// 	
	// 	pMeshRenderer->SetMesh(meshB92fsB);
	// 	pMeshRenderer->SetMaterial(0, matB92fsB);
	// 	pMeshRenderer->SetMaterial(1, matB92fsBx300Body);
	// 	pMeshRenderer->SetMaterial(2, matB92fsBx300Lamp);
	// 	
	// 	pMeshRenderer->SetArmature(arma_usp);
	// 	// pMeshRenderer->PlayAnimation("usp_idle", true);
	// 	pMeshRenderer->PlayAnimation("usp_reload", true);
	// 	// pMeshRenderer->PlayAnimation("usp_shoot", true);
	// 	// pMeshRenderer->PlayAnimation("usp_run", true);
	// }
	
	// 물리엔진 테스트 박스
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(-7.5f, 4.5f, -0.0f);
		// pGameObject->m_transform.SetRotationEuler(XMConvertToRadians(20.0f), 0, 0);
		// pGameObject->m_transform.SetRotationEuler(0, XMConvertToRadians(20.0f), 0);
		// pGameObject->m_transform.SetRotationEuler(0, 0, XMConvertToRadians(20.0f));
	
		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(m_meshPaperBox);
		pMeshRenderer->SetMaterial(0, m_matPaperBox);
	
		std::shared_ptr<BoxCollider> collider = std::make_shared<BoxCollider>(m_meshPaperBox->GetAabb().Extents);
		ComponentHandle<Rigidbody> hRigidbody = pGameObject->AddComponent<Rigidbody>(collider, m_meshPaperBox->GetAabb().Center);
		Rigidbody* pRigidbody = hRigidbody.ToPtr();
		pRigidbody->SetFriction(1.0);
	}


	// Static Meshes
	CreateWoodenBox10090(XMFLOAT3(2.3f, 0.0f, 24.2f));
	CreateWoodenBox10090(XMFLOAT3(2.3f, 0.0f, 23.2f));
	CreateWoodenBox10090(XMFLOAT3(2.2f, 0.9f, 24.2f));
	CreateWoodenBox10090(XMFLOAT3(2.2f, 0.9f, 23.2f));
	CreateWoodenBox10090(XMFLOAT3(2.2f, 1.8f, 23.2f));

	CreateWoodenBox10090(XMFLOAT3(6.445f, 0.0f, 19.32f), XMFLOAT3(0.0f, XMConvertToRadians(45), 0.0f));
	CreateWoodenBox9070(XMFLOAT3(6.445f, 0.9f, 19.25f), XMFLOAT3(0.0f, XMConvertToRadians(45), 0.0f));

	CreateWoodenBox8060(XMFLOAT3(-6.95f, 0.0f, 12.85f));
	CreateWoodenBox9070(XMFLOAT3(-6.1f, 0.0f, 12.9f));
	CreateWoodenBox9070(XMFLOAT3(-7.0f, 0.0f, 12.0f));
	CreateWoodenBox8060(XMFLOAT3(-6.15f, 0.0f, 12.05f));

	CreateWoodenBox9070(XMFLOAT3(-7.775f, 0.0f, 6.9f));
	CreateWoodenBox9070(XMFLOAT3(-7.875f, 0.0f, 6.0f));

	CreateWoodenBox8060(XMFLOAT3(6.05f, 0.0f, 3.85f));
	CreateWoodenBox9070(XMFLOAT3(6.9f, 0.0f, 3.9f));
	CreateWoodenBox9070(XMFLOAT3(6.0f, 0.0f, 3.0f));
	CreateWoodenBox8060(XMFLOAT3(6.85f, 0.0f, 3.05f));

	CreateWoodenBox10090(XMFLOAT3(-11.9f, 0.0f, -1.4f));
	CreateWoodenBox10090(XMFLOAT3(-10.9f, 0.9f, -1.4f));
	CreateWoodenBox10090(XMFLOAT3(-10.9f, 0.0f, -2.4f));

	CreateWoodenBox10090(XMFLOAT3(13.5f, 0.0f, -10.5f));
	CreateWoodenBox9070(XMFLOAT3(13.55f, 0.9f, -10.5f));

	CreateWoodenBox10090(XMFLOAT3(-10.9f, 0.0f, -18.0f));
	CreateWoodenBox10090(XMFLOAT3(-9.9f, 0.0f, -18.0f));
	CreateWoodenBox10090(XMFLOAT3(-10.9f, 0.9f, -18.0f));

	CreateWoodenBox10090(XMFLOAT3(9.9f, 0.0f, -19.2f));
	CreateWoodenBox10090(XMFLOAT3(10.9f, 0.0f, -19.2f));
	CreateWoodenBox9070(XMFLOAT3(10.945f, 0.9f, -19.15f));

	CreateWoodenBox10090(XMFLOAT3(1.2f, 0.0f, -21.7f));
	CreateWoodenBox10090(XMFLOAT3(1.2f, 0.9f, -21.7f));

	CreateWoodenBox10090(XMFLOAT3(3.4f, 0.0f, -23.4f), XMFLOAT3(0.0f, XMConvertToRadians(45.0f), 0.0f));
	CreateWoodenBox10090(XMFLOAT3(3.4f, 0.9f, -23.4f), XMFLOAT3(0.0f, XMConvertToRadians(45.0f), 0.0f));

	CreateWoodenBox10090(XMFLOAT3(12.3f, 2.6f, -23.4f));
	CreateWoodenBox10090(XMFLOAT3(13.3f, 2.6f, -23.4f));
	CreateWoodenBox10090(XMFLOAT3(12.3f, 2.6f, -24.4f));
	CreateWoodenBox10090(XMFLOAT3(12.3f, 3.5f, -24.4f));
	CreateWoodenBox10090(XMFLOAT3(13.3f, 3.5f, -24.4f));


	CreateShortContainer(XMFLOAT3(9.5598f, 0.0f, 19.885f), XMFLOAT3(0.0f, XMConvertToRadians(45.0f), 0.0f));
	CreateLongContainer(XMFLOAT3(4.5371f, 0.0f, 18.681f), XMFLOAT3(0.0f, XMConvertToRadians(-45.0f), 0.0f));

	CreateLongContainer(XMFLOAT3(-7.7f, 0.0f, 20.2f), XMFLOAT3(0.0f, XMConvertToRadians(90.0f), 0.0f));

	CreateLongContainer(XMFLOAT3(-12.7f, 0.0f, 11.2f));

	CreateLongContainer(XMFLOAT3(2.3f, 0.0f, 10.2f), XMFLOAT3(0.0f, XMConvertToRadians(-70.0f), 0.0f));

	CreateShortContainer(XMFLOAT3(-10.9f, 0.0f, 0.4f), XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f));
	CreateShortContainer(XMFLOAT3(-9.1f, 0.0f, -4.0f));
	CreateShortContainer(XMFLOAT3(-4.7f, 0.0f, -2.2f), XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f));

	CreateShortContainer(XMFLOAT3(1.7864f, 0.0f, -7.1945f), XMFLOAT3(0.0f, XMConvertToRadians(-105.0f), 0.0f));
	CreateShortContainer(XMFLOAT3(5.7f, 0.0f, -4.8f), XMFLOAT3(0.0f, XMConvertToRadians(-195.0f), 0.0f));
	CreateShortContainer(XMFLOAT3(9.6136f, 0.0f, -2.4055f), XMFLOAT3(0.0f, XMConvertToRadians(-105.0f), 0.0f));

	CreateShortContainer(XMFLOAT3(-1.3f, 0.0f, -14.8f), XMFLOAT3(0.0f, XMConvertToRadians(-105.0f), 0.0f));

	CreateLongContainer(XMFLOAT3(-12.7f, 0.0f, -18.3f));
	CreateShortContainer(XMFLOAT3(-10.9f, 0.0f, -23.6f), XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f));

	CreateShortContainer(XMFLOAT3(-0.5f, 0.0f, -23.6f), XMFLOAT3(0.0f, XMConvertToRadians(-90.0f), 0.0f));

	CreateLongContainer(XMFLOAT3(10.0f, 0.0f, -17.4f), XMFLOAT3(0.0f, XMConvertToRadians(90.0f), 0.0f));
	CreateShortContainer(XMFLOAT3(12.7f, 0.0f, -21.8f));

	CreateOpenContainer1(XMFLOAT3(12.7f, 0.0f, 8.0f));

	CreateOpenContainer2(XMFLOAT3(-5.6f, 1.568f, 1.3f), XMFLOAT3(XMConvertToRadians(30.0f), XMConvertToRadians(45.0f), 0.0f));

	CreateBarrel1(XMFLOAT3(0.0f, 0.2f, 0.0f));
	CreateBarrel2(XMFLOAT3(0.0f, 0.0f, -1.0f));
	CreateBarrel3(XMFLOAT3(-1.0f, 0.0f, 0.0f));

	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(0.0f, 0.0f, 24.6f);

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFrame);
		pMeshRenderer->SetMaterial(0, matHouseFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(0.0f, 0.0f, 13.3f);

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFrame);
		pMeshRenderer->SetMaterial(0, matHouseFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(0.0f, 0.0f, 2.0f);

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFrame);
		pMeshRenderer->SetMaterial(0, matHouseFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(0.0f, 0.0f, -12.2f);

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFrame);
		pMeshRenderer->SetMaterial(0, matHouseFrame);
	}
	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Frame");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(0.0f, 0.0f, -24.6f);

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFrame);
		pMeshRenderer->SetMaterial(0, matHouseFrame);
	}

	CreateHouseSideWall(XMFLOAT3(-14.1f, 0.0f, 0.0f));
	CreateHouseSideWall(XMFLOAT3(14.1f, 0.0f, 0.0f));

	CreateRedTeamBase(XMFLOAT3(0.0f, 0.0f, 25.0f));
	CreateRedBaseWall(XMFLOAT3(0.0f, 0.0f, 25.0f));

	CreateBlueBaseWall(XMFLOAT3(0.0f, 0.0f, -25.0f));
	CreateBlueTeamBase(XMFLOAT3(0.0f, 0.0f, -25.0f));


	CreatePaperBox(XMFLOAT3(-9.8f, 0.0f, -25.6f));
	CreatePaperBox(XMFLOAT3(-9.8f, 0.0f, -26.6f));

	CreateHouseRoof(XMFLOAT3(0.0f, 5.8f, 0.0f));
	{
		GameObjectHandle hGameObject = CreateGameObject(L"House Floor");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(0.0f, 0.0f, 0.0f);

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFloor);
		pMeshRenderer->SetMaterial(0, matConcrete2);
		
		std::shared_ptr<StaticPlaneCollider> collider = std::make_shared<StaticPlaneCollider>(XMFLOAT3(0.0f, 1.0f, 0.0f), 0.0f);
		pGameObject->AddComponent<Rigidbody>(collider).ToPtr()->SetBodyType(RigidbodyType::Static);
		// constexpr XMFLOAT3 FLOOR_COLLIDER_HALF_EXTENTS(50.0f, 0.5f, 50.0f);
		// std::shared_ptr<BoxCollider> collider = std::make_shared<BoxCollider>(FLOOR_COLLIDER_HALF_EXTENTS);
		// pGameObject->AddComponent<Rigidbody>(collider, XMFLOAT3(0.0f, -FLOOR_COLLIDER_HALF_EXTENTS.y, 0.0f)).ToPtr()->SetBodyType(RigidbodyType::Static);
	}


	// Skybox
	{
		Texture2D skybox = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\skybox\\sky27.dds", false);
		RenderSettings::GetInstance()->SetSkybox(skybox);

		RenderSettings::GetInstance()->SetAmbientLightColor(ColorsLinear::White);
		RenderSettings::GetInstance()->SetAmbientLightIntensity(0.025f);
	}
}
