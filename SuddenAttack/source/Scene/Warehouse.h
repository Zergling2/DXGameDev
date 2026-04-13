#pragma once

#include <ZergEngine\ZergEngine.h>

class Warehouse : public ze::IScene
{
	ZE_DECLARE_SCENE(Warehouse);
public:
	virtual void OnLoadScene() override;
private:
	void CreateShortContainer(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateLongContainer(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateWoodenBox8060(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateWoodenBox9070(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateWoodenBox10090(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreatePaperBox(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateOpenContainer1(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateOpenContainer2(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateHouseSideWall(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateRedTeamBase(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateRedBaseWall(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateBlueTeamBase(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateBlueBaseWall(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateHouseRoof(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
private:
	std::shared_ptr<ze::StaticMesh> m_meshShortContainer;
	std::shared_ptr<ze::Material> m_matShortContainer;
	std::shared_ptr<ze::ICollider> m_colliderShortContainer;

	std::shared_ptr<ze::StaticMesh> m_meshLongContainer;
	std::shared_ptr<ze::Material> m_matLongContainer;
	std::shared_ptr<ze::ICollider> m_colliderLongContainer;

	std::shared_ptr<ze::StaticMesh> m_meshOpenContainer1;
	std::shared_ptr<ze::Material> m_matOpenContainer1;
	std::shared_ptr<ze::ICollider> m_colliderOpenContainer1[2];	// collider shape x2

	std::shared_ptr<ze::StaticMesh> m_meshOpenContainer2;
	std::shared_ptr<ze::Material> m_matOpenContainer2;
	std::shared_ptr<ze::ICollider> m_colliderOpenContainer2[2];	// collider shape x2


	std::shared_ptr<ze::StaticMesh> m_meshWoodenBox8060;
	std::shared_ptr<ze::StaticMesh> m_meshWoodenBox9070;
	std::shared_ptr<ze::StaticMesh> m_meshWoodenBox10090;
	std::shared_ptr<ze::ICollider> m_colliderWoodenBox8060;
	std::shared_ptr<ze::ICollider> m_colliderWoodenBox9070;
	std::shared_ptr<ze::ICollider> m_colliderWoodenBox10090;
	std::shared_ptr<ze::Material> m_matWoodenBox;

	std::shared_ptr<ze::StaticMesh> m_meshPaperBox;
	std::shared_ptr<ze::Material> m_matPaperBox;
	std::shared_ptr<ze::ICollider> m_colliderPaperBox;

	std::shared_ptr<ze::StaticMesh> m_meshHouseSideWall;
	std::shared_ptr<ze::Material> m_matHouseSideWall[2];
	std::shared_ptr<ze::ICollider> m_colliderHouseSideWall;

	std::shared_ptr<ze::StaticMesh> m_meshHouseRedBase;
	std::shared_ptr<ze::Material> m_matHouseRedBase[2];
	std::shared_ptr<ze::ICollider> m_colliderHouseRedBase[2];

	std::shared_ptr<ze::StaticMesh> m_meshHouseRedBaseWall;
	std::shared_ptr<ze::Material> m_matHouseRedBaseWall[3];
	std::shared_ptr<ze::ICollider> m_colliderHouseRedBaseWall[3];

	std::shared_ptr<ze::StaticMesh> m_meshHouseBlueBase;
	std::shared_ptr<ze::Material> m_matHouseBlueBase[3];
	std::shared_ptr<ze::ICollider> m_colliderHouseBlueBase[8];

	std::shared_ptr<ze::StaticMesh> m_meshHouseBlueBaseWall;
	std::shared_ptr<ze::Material> m_matHouseBlueBaseWall[4];
	std::shared_ptr<ze::ICollider> m_colliderHouseBlueBaseWall[2];

	std::shared_ptr<ze::StaticMesh> m_meshHouseRoof;
	std::shared_ptr<ze::Material> m_matHouseRoof;
	std::shared_ptr<ze::ICollider> m_colliderHouseRoof;
};
