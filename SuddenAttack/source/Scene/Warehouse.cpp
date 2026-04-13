#include "Warehouse.h"
#include "..\Script\FirstPersonController.h"
#include "..\Script\UIHandler.h"
#include "..\Script\ThirdPersonCharacter.h"
#include "..\Script\CollisionEventTest.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(Warehouse);

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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderShortContainer, rbLocalPos, rbLocalRot);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderLongContainer, rbLocalPos, rbLocalRot);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderWoodenBox8060, rbLocalPos, rbLocalRot);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderWoodenBox9070, rbLocalPos, rbLocalRot);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderWoodenBox10090, rbLocalPos, rbLocalRot);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderPaperBox, rbLocalPos, rbLocalRot);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer1[0], rbLocalPos[0], rbLocalRot[0]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer1[0], rbLocalPos[1], rbLocalRot[1]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer1[0], rbLocalPos[2], rbLocalRot[2]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer1[0], rbLocalPos[3], rbLocalRot[3]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer1[1], rbLocalPos[4], rbLocalRot[4]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer1[1], rbLocalPos[5], rbLocalRot[5]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer1[1], rbLocalPos[6], rbLocalRot[6]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer1[1], rbLocalPos[7], rbLocalRot[7]);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer2[0], rbLocalPos[0], rbLocalRot[0]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer2[0], rbLocalPos[1], rbLocalRot[1]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer2[0], rbLocalPos[2], rbLocalRot[2]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer2[0], rbLocalPos[3], rbLocalRot[3]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer2[1], rbLocalPos[4], rbLocalRot[4]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer2[1], rbLocalPos[5], rbLocalRot[5]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer2[1], rbLocalPos[6], rbLocalRot[6]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderOpenContainer2[1], rbLocalPos[7], rbLocalRot[7]);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseSideWall, rbLocalPos, rbLocalRot);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBase[0], rbLocalPos[0], rbLocalRot[0]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBase[1], rbLocalPos[1], rbLocalRot[1]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBase[1], rbLocalPos[2], rbLocalRot[2]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBase[1], rbLocalPos[3], rbLocalRot[3]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBase[1], rbLocalPos[4], rbLocalRot[4]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBase[1], rbLocalPos[5], rbLocalRot[5]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBase[1], rbLocalPos[6], rbLocalRot[6]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBase[1], rbLocalPos[7], rbLocalRot[7]);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBaseWall[0], rbLocalPos[0], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBaseWall[1], rbLocalPos[1], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBaseWall[1], rbLocalPos[2], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBaseWall[1], rbLocalPos[3], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBaseWall[2], rbLocalPos[4], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRedBaseWall[2], rbLocalPos[5], rbLocalRot);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBase[0], rbLocalPos[0], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBase[1], rbLocalPos[1], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBase[1], rbLocalPos[2], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBase[2], rbLocalPos[3], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBase[3], rbLocalPos[4], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBase[4], rbLocalPos[5], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBase[4], rbLocalPos[6], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBase[5], rbLocalPos[7], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBase[6], rbLocalPos[8], rbLocalRot);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBase[7], rbLocalPos[9], rbLocalRot);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBaseWall[0], rbLocalPos[0], rbLocalRot[0]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBaseWall[1], rbLocalPos[1], rbLocalRot[1]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBaseWall[1], rbLocalPos[2], rbLocalRot[2]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseBlueBaseWall[1], rbLocalPos[3], rbLocalRot[3]);
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

	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRoof, rbLocalPos[0], rbLocalRot[0]);
	pGameObject->AddComponent<StaticRigidbody>(m_colliderHouseRoof, rbLocalPos[1], rbLocalRot[1]);
}

void Warehouse::OnLoadScene()
{
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

	// ## Main Camera
	GameObject* pGameObjectPlayerCamera = nullptr;
	{
		GameObjectHandle hGameObjectPlayerCamera = CreateGameObject(L"Main Camera");

		pGameObjectPlayerCamera = hGameObjectPlayerCamera.ToPtr();

		ComponentHandle<Camera> hCamera = pGameObjectPlayerCamera->AddComponent<Camera>();
		Camera* pCamera = hCamera.ToPtr();
		pCamera->SetDepth(0);
		pCamera->SetFieldOfView(82);
		pCamera->SetClippingPlanes(0.1f, 300.0f);

		pGameObjectPlayerCamera->AddComponent<FirstPersonController>();		// 1인칭 카메라 조작
	}
	
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

	GameObjectHandle hGameObjectUIHandler = CreateGameObject();
	GameObject* pGameObjectUIHandler = hGameObjectUIHandler.ToPtr();
	ComponentHandle<UIHandler> hScriptUIHandler = pGameObjectUIHandler->AddComponent<UIHandler>();
	UIHandler* pScriptUIHandler = hScriptUIHandler.ToPtr();

	// ## UI
	{
		UIObjectHandle hImageCrosshair = CreateImage();
		pScriptUIHandler->m_hImageCrosshair = hImageCrosshair;
		Image* pImageCrosshair = static_cast<Image*>(hImageCrosshair.ToPtr());
		pImageCrosshair->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\crosshair.dds"));
		pImageCrosshair->SetNativeSize(true);
		pImageCrosshair->m_transform.SetHorizontalAnchor(HorizontalAnchor::Center);
		pImageCrosshair->m_transform.SetVerticalAnchor(VerticalAnchor::VCenter);

		UIObjectHandle hImageHealthBackground = CreateImage();
		pScriptUIHandler->m_hImageHealthBackground = hImageHealthBackground;
		Image* pImageHealthBackground = static_cast<Image*>(hImageHealthBackground.ToPtr());
		pImageHealthBackground->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\health.png"));
		pImageHealthBackground->SetNativeSize(true);
		pImageHealthBackground->m_transform.SetPosition(pImageHealthBackground->GetHalfSizeX() + 4, pImageHealthBackground->GetHalfSizeY() + 4);
		pImageHealthBackground->m_transform.SetHorizontalAnchor(HorizontalAnchor::Left);
		pImageHealthBackground->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);

		UIObjectHandle hImageRBUIBackground = CreateImage();
		pScriptUIHandler->m_hImageRBUIBackground = hImageRBUIBackground;
		Image* pImageRBUIBackground = static_cast<Image*>(hImageRBUIBackground.ToPtr());
		pImageRBUIBackground->SetTexture(ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\sprites\\weapon_indicator.png"));
		pImageRBUIBackground->SetNativeSize(true);
		pImageRBUIBackground->m_transform.SetPosition(-pImageRBUIBackground->GetHalfSizeX() - 4, pImageRBUIBackground->GetHalfSizeY() + 4);
		pImageRBUIBackground->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
		pImageRBUIBackground->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);

		UIObjectHandle hTextHP = CreateText();
		pScriptUIHandler->m_hTextHP = hTextHP;
		Text* pTextHP = static_cast<Text*>(hTextHP.ToPtr());
		pTextHP->SetText(L"100");
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
		pScriptUIHandler->m_hTextAP = hTextAP;
		Text* pTextAP = static_cast<Text*>(hTextAP.ToPtr());
		pTextAP->SetText(L"100");
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
		pScriptUIHandler->m_hTextPoint = hTextPoint;
		Text* pTextPoint = static_cast<Text*>(hTextPoint.ToPtr());
		pTextPoint->SetText(L"126P");
		pTextPoint->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
		pTextPoint->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextPoint->m_transform.SetPosition(pImageRBUIBackground->m_transform.GetPosition());
		pTextPoint->m_transform.Translate(-45, 34);
		pTextPoint->SetSize(128, 32);
		pTextPoint->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextPoint->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextPoint->SetColor(ColorsLinear::White);
		pTextPoint->GetTextFormat().SetSize(22);
		pTextPoint->GetTextFormat().SetFontFamilyName(L"Agency FB");
		pTextPoint->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
		pTextPoint->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
		pTextPoint->ApplyTextFormat();

		UIObjectHandle hTextWeaponName = CreateText();
		pScriptUIHandler->m_hTextWeaponName = hTextWeaponName;
		Text* pTextWeaponName = static_cast<Text*>(hTextWeaponName.ToPtr());
		pTextWeaponName->SetText(L"M16");
		pTextWeaponName->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
		pTextWeaponName->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextWeaponName->m_transform.SetPosition(pImageRBUIBackground->m_transform.GetPosition());
		pTextWeaponName->m_transform.TranslateY(6);
		pTextWeaponName->SetSize(XMFLOAT2(200, 32));
		pTextWeaponName->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextWeaponName->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextWeaponName->SetColor(ColorsLinear::White);
		pTextWeaponName->GetTextFormat().SetSize(22);
		pTextWeaponName->GetTextFormat().SetFontFamilyName(L"Agency FB");
		pTextWeaponName->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
		pTextWeaponName->ApplyTextFormat();

		UIObjectHandle hTextAmmo = CreateText();
		pScriptUIHandler->m_hTextAmmo = hTextAmmo;
		Text* pTextAmmo = static_cast<Text*>(hTextAmmo.ToPtr());
		pTextAmmo->SetText(L"30 / 90");
		pTextAmmo->m_transform.SetHorizontalAnchor(HorizontalAnchor::Right);
		pTextAmmo->m_transform.SetVerticalAnchor(VerticalAnchor::Bottom);
		pTextAmmo->m_transform.SetPosition(pImageRBUIBackground->m_transform.GetPosition());
		pTextAmmo->m_transform.Translate(16, -28);
		pTextAmmo->SetSize(XMFLOAT2(128, 40));
		pTextAmmo->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		pTextAmmo->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		pTextAmmo->SetColor(ColorsLinear::White);
		pTextAmmo->GetTextFormat().SetSize(34);
		pTextAmmo->GetTextFormat().SetFontFamilyName(L"Agency FB");
		pTextAmmo->GetTextFormat().SetStyle(DWRITE_FONT_STYLE_ITALIC);
		pTextAmmo->GetTextFormat().SetWeight(DWRITE_FONT_WEIGHT_ULTRA_BOLD);
		pTextAmmo->ApplyTextFormat();
	}





	// ## MATERIALS
	std::shared_ptr<Material> matAsphault = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matAsphault->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.8f), 1.0f));
	XMStoreFloat4A(&matAsphault->m_specular, XMVectorSetW(ColorsLinear::Black, 4.0f));
	matAsphault->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\asphault\\AsphaultStreet_d.tga");
	matAsphault->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\textures\\asphault\\AsphaultStreet_n.tga");

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


	std::shared_ptr<StaticMesh> meshHouseFrame = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\house_frame.obj").m_staticMeshes[0];
	std::shared_ptr<StaticMesh> meshHouseFloor = ResourceLoader::GetInstance()->LoadModel(L"resources\\maps\\warehouse\\house_floor.obj").m_staticMeshes[0];




	constexpr XMFLOAT3 FPSARM_POS(0.0f, -0.2f, -0.06f);
	constexpr XMFLOAT3 M16_PV_OFFSET(0.1f, -0.04f, 0.23f);
	constexpr XMFLOAT3 M16_PV_POS(FPSARM_POS.x + M16_PV_OFFSET.x, FPSARM_POS.y + M16_PV_OFFSET.y, FPSARM_POS.z + M16_PV_OFFSET.z);
	// FPS Arms
	{
		ModelData md_fpsarms = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\characters\\steven\\fps\\fpsarms.glb");
		std::shared_ptr<SkinnedMesh> mesh_fpsarms = md_fpsarms.m_skinnedMeshes[0];
		std::shared_ptr<Armature> arma_fpsarms = md_fpsarms.m_armatures[0];
		bool grouping = arma_fpsarms->CreateBoneGroupByRootBoneName("whole", "Root");
		assert(grouping);

		GameObjectHandle hGameObject = CreateGameObject(L"FPS Arms");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetParent(&pGameObjectPlayerCamera->m_transform);

		pGameObject->m_transform.SetPosition(FPSARM_POS);

		ComponentHandle<SkinnedMeshRenderer> hSkinnedMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
		SkinnedMeshRenderer* pSkinnedMeshRenderer = hSkinnedMeshRenderer.ToPtr();

		auto matArms = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matArms->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 1.0f));
		XMStoreFloat4A(&matArms->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
		matArms->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\fps\\tex_diffuse.jpg");

		pSkinnedMeshRenderer->SetMesh(mesh_fpsarms);
		pSkinnedMeshRenderer->SetMaterial(0, matArms);

		pSkinnedMeshRenderer->SetArmature(arma_fpsarms);
		// pSkinnedMeshRenderer->PlayAnimation("arms_idle_m16a1", true);
		// pSkinnedMeshRenderer->PlayAnimation("arms_idle_usp", true);
		// pSkinnedMeshRenderer->PlayAnimation("arms_reload_m16a1", true);
		pSkinnedMeshRenderer->PlayAnimation("arms_reload_usp", true);
		// pSkinnedMeshRenderer->PlayAnimation("arms_shoot_m16a1", true);
		// pSkinnedMeshRenderer->PlayAnimation("arms_shoot_usp", true);
		// pSkinnedMeshRenderer->PlayAnimation("arms_run_m16a1", true);
		// pSkinnedMeshRenderer->PlayAnimation("arms_run_usp", true);
	}

	auto matSTANAG30Rds = ResourceLoader::GetInstance()->CreateMaterial();
	XMStoreFloat4A(&matSTANAG30Rds->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.05f), 1.0f));
	XMStoreFloat4A(&matSTANAG30Rds->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 2.0f));
	ModelData md_m16a1_pv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\M16A1\\m16a1_pv.glb");
	std::shared_ptr<SkinnedMesh> mesh = md_m16a1_pv.m_skinnedMeshes[0];
	std::shared_ptr<Armature> arma_m16a1 = md_m16a1_pv.m_armatures[0];
	bool grouping = arma_m16a1->CreateBoneGroupByRootBoneName("default", "bone_m16a1_body");	// 디폴트 그룹은 필수
	assert(grouping);
	// Animated Weapons(m16a1)
	// {
	// 	GameObjectHandle hGameObject = CreateGameObject(L"M16A1");
	// 	GameObject* pGameObject = hGameObject.ToPtr();
	// 	pGameObject->m_transform.SetParent(&pMainCamera->m_transform);
	// 	
	// 	pGameObject->m_transform.SetPosition(M16_PV_POS);
	// 	
	// 	ComponentHandle<SkinnedMeshRenderer> hMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
	// 	SkinnedMeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	// 	
	// 	// 재질 설정
	// 	auto matM16A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
	// 	XMStoreFloat4A(&matM16A1Receiver->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
	// 	XMStoreFloat4A(&matM16A1Receiver->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 2.0f));
	// 	matM16A1Receiver->m_diffuseMap =
	// 		ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\receiver_diffuse.jpg");
	// 	matM16A1Receiver->m_normalMap =
	// 		ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\receiver_normal.jpg");
	// 	
	// 	auto matM16A1Furniture = ResourceLoader::GetInstance()->CreateMaterial();
	// 	XMStoreFloat4A(&matM16A1Furniture->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
	// 	XMStoreFloat4A(&matM16A1Furniture->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 4.0f));
	// 	matM16A1Furniture->m_diffuseMap =
	// 		ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\furniture_diffuse.jpg");
	// 	matM16A1Furniture->m_normalMap =
	// 		ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\M16A1\\textures\\furniture_normal.jpg");
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

	// Animated Weapons(m4a1)
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
	// 	auto matM4A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
	// 	XMStoreFloat4A(&matM4A1Receiver->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
	// 	XMStoreFloat4A(&matM4A1Receiver->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
	// 	matM4A1Receiver->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_diffuse.png");
	// 	matM4A1Receiver->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_normal.png");
	// 	auto matM4A1Furniture = ResourceLoader::GetInstance()->CreateMaterial();
	// 	XMStoreFloat4A(&matM4A1Furniture->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
	// 	XMStoreFloat4A(&matM4A1Furniture->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 8.0f));
	// 	matM4A1Furniture->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_diffuse.png");
	// 	matM4A1Furniture->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_normal.png");
	// 	auto matRearSight = ResourceLoader::GetInstance()->CreateMaterial();
	// 	XMStoreFloat4A(&matRearSight->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
	// 	XMStoreFloat4A(&matRearSight->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
	// 	matRearSight->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_diffuse.png");
	// 	matRearSight->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_normal.png");
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


	// Animated Weapons(usp)
	constexpr XMFLOAT3 USP_PV_OFFSET(0.04f, -0.01f, 0.41f);
	constexpr XMFLOAT3 USP_PV_POS(FPSARM_POS.x + USP_PV_OFFSET.x, FPSARM_POS.y + USP_PV_OFFSET.y, FPSARM_POS.z + USP_PV_OFFSET.z);
	{
		ModelData md_usp_pv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\usp\\usp_pv.glb");
		std::shared_ptr<SkinnedMesh> mesh_usp = md_usp_pv.m_skinnedMeshes[0];
		std::shared_ptr<Armature> arma_usp = md_usp_pv.m_armatures[0];	// USP 뼈대 공유
		bool grouping = arma_usp->CreateBoneGroupByRootBoneName("default", "bone_usp_body");	// 디폴트 그룹은 필수
		assert(grouping);

		GameObjectHandle hGameObjectSecondaryWeaponPV = CreateGameObject(L"C.USP");
		GameObject* pGameObjectSecondaryWeaponPV = hGameObjectSecondaryWeaponPV.ToPtr();
		pGameObjectSecondaryWeaponPV->m_transform.SetParent(&pGameObjectPlayerCamera->m_transform);
		pGameObjectSecondaryWeaponPV->m_transform.SetPosition(USP_PV_POS);

		ComponentHandle<SkinnedMeshRenderer> hMeshRendererSecondaryWeaponPV = pGameObjectSecondaryWeaponPV->AddComponent<SkinnedMeshRenderer>();
		SkinnedMeshRenderer* pMeshRendererSecondaryWeaponPV = hMeshRendererSecondaryWeaponPV.ToPtr();

		// 재질 설정
		auto mat_usp = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&mat_usp->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
		XMStoreFloat4A(&mat_usp->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
		mat_usp->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\usp\\textures\\diffuse.png");
		mat_usp->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\usp\\textures\\normal.png");

		pMeshRendererSecondaryWeaponPV->SetMesh(mesh_usp);
		pMeshRendererSecondaryWeaponPV->SetMaterial(0, mat_usp);

		pMeshRendererSecondaryWeaponPV->SetArmature(arma_usp);
		// pMeshRendererSecondaryWeaponPV->PlayAnimation("usp_idle", true);
		pMeshRendererSecondaryWeaponPV->PlayAnimation("usp_reload", true);
		// pMeshRendererSecondaryWeaponPV->PlayAnimation("usp_shoot", true);
		// pMeshRendererSecondaryWeaponPV->PlayAnimation("usp_run", true);
	}

	// Animated Weapons(b92fs black)
	// {
	// 	ModelData md = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\b92fsb\\b92fsb_pv.glb");
	// 	std::shared_ptr<SkinnedMesh> meshB92fsB = md.m_skinnedMeshes[0];
	// 	std::shared_ptr<Armature> arma = md.m_armatures[0];	// USP 뼈대 공유
	// 	bool grouping = arma->CreateBoneGroupByRootBoneName("default", "bone_usp_body");	// 디폴트 그룹은 필수
	// 	assert(grouping);
	// 
	// 	GameObjectHandle hGameObject = CreateGameObject(L"B.92Fs black");
	// 	GameObject* pGameObject = hGameObject.ToPtr();
	// 	pGameObject->m_transform.SetParent(&pMainCamera->m_transform);
	// 
	// 	pGameObject->m_transform.SetPosition(USP_PV_POS);
	// 
	// 	ComponentHandle<SkinnedMeshRenderer> hMeshRenderer = pGameObject->AddComponent<SkinnedMeshRenderer>();
	// 	SkinnedMeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	// 
	// 	// 재질 설정
	// 	auto matB92fsB = ResourceLoader::GetInstance()->CreateMaterial();
	// 	XMStoreFloat4A(&matB92fsB->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	// 	XMStoreFloat4A(&matB92fsB->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	// 	matB92fsB->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\diffuse.png");
	// 	matB92fsB->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\normal.png");
	// 
	// 	auto matB92fsBx300Body = ResourceLoader::GetInstance()->CreateMaterial();
	// 	XMStoreFloat4A(&matB92fsBx300Body->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	// 	XMStoreFloat4A(&matB92fsBx300Body->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	// 	matB92fsBx300Body->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_body_diffuse.jpg");
	// 	matB92fsBx300Body->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_body_normal.png");
	// 
	// 	auto matB92fsBx300Lamp = ResourceLoader::GetInstance()->CreateMaterial();
	// 	XMStoreFloat4A(&matB92fsBx300Lamp->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
	// 	XMStoreFloat4A(&matB92fsBx300Lamp->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
	// 	matB92fsBx300Lamp->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_lamp_diffuse.jpg");
	// 
	// 	pMeshRenderer->SetMesh(meshB92fsB);
	// 	pMeshRenderer->SetMaterial(0, matB92fsB);
	// 	pMeshRenderer->SetMaterial(1, matB92fsBx300Body);
	// 	pMeshRenderer->SetMaterial(2, matB92fsBx300Lamp);
	// 
	// 	pMeshRenderer->SetArmature(arma);
	// 	// pMeshRenderer->PlayAnimation("usp_idle", true);
	// 	pMeshRenderer->PlayAnimation("usp_reload", true);
	// 	// pMeshRenderer->PlayAnimation("usp_shoot", true);
	// 	// pMeshRenderer->PlayAnimation("usp_run", true);
	// }
	// 
	// 물리엔진 테스트 박스
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Box");
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(-0.5f, 6.0f, 0.5f);
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
		pRigidbody->SetFreezePosition(true, false, true);
		pRigidbody->SetFreezeRotation(true, true, true);
		pRigidbody->SetFriction(0.5);
	}
	
	// Kinematic 발판 테스트
	GameObjectHandle hKinematicFootboard;
	{
		GameObjectHandle hGameObject = CreateGameObject(L"Footboard");
		hKinematicFootboard = hGameObject;
	
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(-1.0f, 0.5f, 1.0f);
		
		// ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		// MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		// pMeshRenderer->SetMesh(meshBox);
		// pMeshRenderer->SetMaterial(0, matPaperBox);
		
		std::shared_ptr<BoxCollider> collider = std::make_shared<BoxCollider>(XMFLOAT3(1.0f, 0.25f, 1.0f));
		ComponentHandle<Rigidbody> hRigidbody = pGameObject->AddComponent<Rigidbody>(collider);
		Rigidbody* pRigidbody = hRigidbody.ToPtr();
		pRigidbody->SetKinematic(true);
	}
	
	
	// CollisionTrigger 테스트
	GameObjectHandle hCollisionTriggerObj;
	{
		GameObjectHandle hGameObject = CreateGameObject(L"CollisionTrigger");
		hCollisionTriggerObj = hGameObject;
	
		GameObject* pGameObject = hGameObject.ToPtr();
		pGameObject->m_transform.SetPosition(0.0f, 3.0f, 0.0f);
	
		std::shared_ptr<BoxCollider> collider = std::make_shared<BoxCollider>(XMFLOAT3(0.5f, 0.5f, 0.5f));
		ComponentHandle<Rigidbody> hCollisionTrigger = pGameObject->AddComponent<Rigidbody>(collider);
		Rigidbody* pCollisionTrigger = hCollisionTrigger.ToPtr();
		pCollisionTrigger->SetKinematic(true);
		pCollisionTrigger->SetTrigger(true);
	
		ComponentHandle<CollisionEventTest> hCollisionEventTest = pGameObject->AddComponent<CollisionEventTest>();
		hCollisionEventTest.ToPtr()->m_hRigidbody = hCollisionTrigger;
	}


	// ## Third person character test
	{
		ModelData md_steven_tv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\characters\\steven\\steven.glb");
		std::shared_ptr<SkinnedMesh> meshSteven = md_steven_tv.m_skinnedMeshes[0];
		std::shared_ptr<Armature> armaSteven = md_steven_tv.m_armatures[0];
		bool grouping;
		grouping = armaSteven->CreateBoneGroupByRootBoneName("upper_body", "Spine0");
		assert(grouping);
		grouping = armaSteven->CreateBoneGroupByExcludeGroup("lower_body", "upper_body");
		assert(grouping);
		
		GameObjectHandle hGameObjectThirdPersonCharacter = CreateGameObject(L"steven");
		GameObject* pGameObjectThirdPersonCharacter = hGameObjectThirdPersonCharacter.ToPtr();
		pGameObjectThirdPersonCharacter->m_transform.SetPosition(-1, 0, 1);

		// 캐릭터 스크립트 컴포넌트
		ComponentHandle<ThirdPersonCharacter> hScriptThirdPersonCharacter = pGameObjectThirdPersonCharacter->AddComponent<ThirdPersonCharacter>();
		ThirdPersonCharacter* pScriptThirdPersonCharacter = hScriptThirdPersonCharacter.ToPtr();

		ComponentHandle<SkinnedMeshRenderer> hSkinnedMeshRendererThirdPersonCharacter = pGameObjectThirdPersonCharacter->AddComponent<SkinnedMeshRenderer>();
		pScriptThirdPersonCharacter->m_hSkinnedMeshRendererThirdPersonCharacter = hSkinnedMeshRendererThirdPersonCharacter;	// 컴포넌트 핸들을 멤버로 저장
		SkinnedMeshRenderer* pSkinnedMeshRendererThirdPersonCharacter = hSkinnedMeshRendererThirdPersonCharacter.ToPtr();


		auto matBody = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matBody->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
		XMStoreFloat4A(&matBody->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.1f), 1.0f));
		matBody->m_specular.w = 4.0f;
		matBody->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\textures\\body.png");
		
		auto matSuit = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matSuit->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.6f), 1.0f));
		XMStoreFloat4A(&matSuit->m_specular, XMVectorSetW(ColorsLinear::Black, 1.0f));
		matSuit->m_specular.w = 4.0f;
		matSuit->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\textures\\suit_diffuse.png");
		matSuit->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\characters\\steven\\textures\\suit_normal.png");
		
		auto matShoes = ResourceLoader::GetInstance()->CreateMaterial();
		XMStoreFloat4A(&matShoes->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		XMStoreFloat4A(&matShoes->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
		matShoes->m_specular.w = 4.0f;
		
		pSkinnedMeshRendererThirdPersonCharacter->SetMesh(meshSteven);
		pSkinnedMeshRendererThirdPersonCharacter->SetMaterial(0, matBody);
		pSkinnedMeshRendererThirdPersonCharacter->SetMaterial(1, matSuit);
		pSkinnedMeshRendererThirdPersonCharacter->SetMaterial(2, matShoes);

		pSkinnedMeshRendererThirdPersonCharacter->SetArmature(armaSteven);

		// pSkinnedMeshRendererThirdPersonCharacter->PlayAnimation("run", false);
		// pSkinnedMeshRendererThirdPersonCharacter->PlayAnimation("reload_rifle", true);
		// pSkinnedMeshRendererThirdPersonCharacter->PlayAnimation("aim_rifle", true);
		// pSkinnedMeshRendererThirdPersonCharacter->PlayAnimation("aim_pistol", true);
		// pSkinnedMeshRendererThirdPersonCharacter->PlayAnimation("reload_pistol", true);

		pSkinnedMeshRendererThirdPersonCharacter->PlayGroupAnimation("aim_rifle", "lower_body", true);
		// pSkinnedMeshRendererThirdPersonCharacter->PlayGroupAnimation("shoot_pistol", "upper_body", true);
		// pSkinnedMeshRendererThirdPersonCharacter->PlayGroupAnimation("reload_pistol", "upper_body", true
		pSkinnedMeshRendererThirdPersonCharacter->PlayGroupAnimation("aim_rifle", "upper_body", true);


		// 주 무기 3인칭 핸들링 테스트
		GameObjectHandle hGameObjectTVWeaponBase = CreateGameObject(L"tv weapon base");
		pScriptThirdPersonCharacter->m_hGameObjectTVWeaponBase = hGameObjectTVWeaponBase;		// 오브젝트 핸들 저장
		GameObject* pGameObjectTVWeaponBase = hGameObjectTVWeaponBase.ToPtr();
		pGameObjectTVWeaponBase->m_transform.SetParent(&pGameObjectThirdPersonCharacter->m_transform);

		
		constexpr XMFLOAT3 PRIMARY_WEAPON_OFFSET(-0.004f, +0.06f, +0.03f);		// Primary weapon local pos
		XMVECTOR primaryWeaponLocalRot = XMQuaternionRotationNormal(Vector3::Up(), XMConvertToRadians(+90));
		primaryWeaponLocalRot = XMQuaternionMultiply(primaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Forward(), XMConvertToRadians(+81)));
		primaryWeaponLocalRot = XMQuaternionMultiply(primaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Right(), XMConvertToRadians(+8)));
		{
			// m4a1_tv모델 메시 & 재질 불러오기
			auto mesh_m4a1_tv = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\m4a1\\m4a1_tv.obj").m_staticMeshes[0];
			auto matM4A1Receiver = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&matM4A1Receiver->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
			XMStoreFloat4A(&matM4A1Receiver->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
			matM4A1Receiver->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_diffuse.png");
			matM4A1Receiver->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\receiver_normal.png");
			auto matM4A1Furniture = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&matM4A1Furniture->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
			XMStoreFloat4A(&matM4A1Furniture->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 8.0f));
			matM4A1Furniture->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_diffuse.png");
			matM4A1Furniture->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\furniture_normal.png");
			auto matRearSight = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&matRearSight->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.15f), 1.0f));
			XMStoreFloat4A(&matRearSight->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 4.0f));
			matRearSight->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_diffuse.png");
			matRearSight->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\m4a1\\textures\\rearsight_normal.png");

			GameObjectHandle hGameObjectTVWeapon = CreateGameObject(L"tv_weapon_pri");
			pScriptThirdPersonCharacter->m_hGameObjectTVWeapons[0] = hGameObjectTVWeapon;		// 오브젝트 핸들 저장
			GameObject* pGameObjectTVWeapon = hGameObjectTVWeapon.ToPtr();
			pGameObjectTVWeapon->m_transform.SetRotationQuaternion(primaryWeaponLocalRot);
			pGameObjectTVWeapon->m_transform.SetPosition(PRIMARY_WEAPON_OFFSET);
			pGameObjectTVWeapon->m_transform.SetParent(&pGameObjectTVWeaponBase->m_transform);

			ComponentHandle<MeshRenderer> hMeshRendererTVWeapon = pGameObjectTVWeapon->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRendererTVWeapon = hMeshRendererTVWeapon.ToPtr();

			// 메시 설정
			pMeshRendererTVWeapon->SetMesh(mesh_m4a1_tv);
			pMeshRendererTVWeapon->SetMaterial(0, matM4A1Receiver);
			pMeshRendererTVWeapon->SetMaterial(1, matM4A1Furniture);
			pMeshRendererTVWeapon->SetMaterial(2, matRearSight);
			pMeshRendererTVWeapon->SetMaterial(3, matSTANAG30Rds);
		}
		
		constexpr XMFLOAT3 SECONDARY_WEAPON_OFFSET(-0.014f, +0.07f, +0.03f);		// SAVE
		XMVECTOR secondaryWeaponLocalRot = XMQuaternionRotationNormal(Vector3::Up(), XMConvertToRadians(+90));
		secondaryWeaponLocalRot = XMQuaternionMultiply(secondaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Forward(), XMConvertToRadians(+81)));
		secondaryWeaponLocalRot = XMQuaternionMultiply(secondaryWeaponLocalRot, XMQuaternionRotationNormal(Vector3::Right(), XMConvertToRadians(-10)));

		{
			auto mesh_b92fsb = ResourceLoader::GetInstance()->LoadModel(L"resources\\models\\weapons\\b92fsb\\b92fsb_tv.obj").m_staticMeshes[0];
			auto mat_b92fsb = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&mat_b92fsb->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
			XMStoreFloat4A(&mat_b92fsb->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
			mat_b92fsb->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\diffuse.png");
			mat_b92fsb->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\normal.png");

			auto mat_b92fsb_x300Body = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&mat_b92fsb_x300Body->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
			XMStoreFloat4A(&mat_b92fsb_x300Body->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
			mat_b92fsb_x300Body->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_body_diffuse.jpg");
			mat_b92fsb_x300Body->m_normalMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_body_normal.png");

			auto mat_b92fsb_x300Lamp = ResourceLoader::GetInstance()->CreateMaterial();
			XMStoreFloat4A(&mat_b92fsb_x300Lamp->m_diffuse, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.2f), 1.0f));
			XMStoreFloat4A(&mat_b92fsb_x300Lamp->m_specular, XMVectorSetW(XMVectorScale(ColorsLinear::White, 0.25f), 4.0f));
			mat_b92fsb_x300Lamp->m_diffuseMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\models\\weapons\\b92fsb\\textures\\x300_lamp_diffuse.jpg");


			GameObjectHandle hGameObjectTVWeapon = CreateGameObject(L"tv_weapon_sec");
			pScriptThirdPersonCharacter->m_hGameObjectTVWeapons[1] = hGameObjectTVWeapon;		// 오브젝트 핸들 저장
			GameObject* pGameObjectTVWeapon = hGameObjectTVWeapon.ToPtr();
			pGameObjectTVWeapon->m_transform.SetPosition(SECONDARY_WEAPON_OFFSET);
			pGameObjectTVWeapon->m_transform.SetRotationQuaternion(secondaryWeaponLocalRot);
			pGameObjectTVWeapon->m_transform.SetParent(&pGameObjectTVWeaponBase->m_transform);

			ComponentHandle<MeshRenderer> hMeshRendererTVWeapon = pGameObjectTVWeapon->AddComponent<MeshRenderer>();
			MeshRenderer* pMeshRendererTVWeapon = hMeshRendererTVWeapon.ToPtr();

			pMeshRendererTVWeapon->SetMesh(mesh_b92fsb);
			pMeshRendererTVWeapon->SetMaterial(0, mat_b92fsb);
			pMeshRendererTVWeapon->SetMaterial(1, mat_b92fsb_x300Body);
			pMeshRendererTVWeapon->SetMaterial(2, mat_b92fsb_x300Lamp);
		}
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

		ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->AddComponent<MeshRenderer>();
		MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
		pMeshRenderer->SetMesh(meshHouseFloor);
		pMeshRenderer->SetMaterial(0, matConcrete2);

		std::shared_ptr<StaticPlaneCollider> collider = std::make_shared<StaticPlaneCollider>();
		auto c = pGameObject->AddComponent<StaticRigidbody>(collider);
	}


	// Skybox
	{
		Texture2D skybox = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\skybox\\sky27.dds", false);
		RenderSettings::GetInstance()->SetSkybox(skybox);

		RenderSettings::GetInstance()->SetAmbientLightColor(ColorsLinear::White);
		RenderSettings::GetInstance()->SetAmbientLightIntensity(0.025f);
	}
}
