#pragma once

#include <ZergEngine\ZergEngine.h>

class Warehouse : public ze::IScene
{
	ZE_DECLARE_SCENE(Warehouse);
public:
	virtual void OnLoadScene() override;
private:
	void CreateClosedContainer(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateClosedLongContainer(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f));
	void CreateWoodenBox(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f));
	void CreatePaperBox(const XMFLOAT3& pos = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& rot = XMFLOAT3(0.0f, 0.0f, 0.0f), const XMFLOAT3& scale = XMFLOAT3(1.0f, 1.0f, 1.0f));
private:
	std::shared_ptr<ze::StaticMesh> m_meshClosedContainer;
	std::shared_ptr<ze::Material> m_matClosedContainer;
	std::shared_ptr<ze::ICollider> m_colliderClosedContainer;
	XMFLOAT3 m_rbLocalPosClosedContainer;
	XMFLOAT4 m_rbLocalRotClosedContainer;

	std::shared_ptr<ze::StaticMesh> m_meshClosedLongContainer;
	std::shared_ptr<ze::Material> m_matClosedLongContainer;
	std::shared_ptr<ze::ICollider> m_colliderClosedLongContainer;
	XMFLOAT3 m_rbLocalPosClosedLongContainer;
	XMFLOAT4 m_rbLocalRotClosedLongContainer;

	std::shared_ptr<ze::StaticMesh> m_meshWoodenBox;
	std::shared_ptr<ze::Material> m_matWoodenBox;
	std::shared_ptr<ze::ICollider> m_colliderWoodenBox;
	XMFLOAT3 m_rbLocalPosWoodenBox;
	XMFLOAT4 m_rbLocalRotWoodenBox;

	std::shared_ptr<ze::StaticMesh> m_meshPaperBox;
	std::shared_ptr<ze::Material> m_matPaperBox;
	std::shared_ptr<ze::ICollider> m_colliderPaperBox;
	XMFLOAT3 m_rbLocalPosPaperBox;
	XMFLOAT4 m_rbLocalRotPaperBox;
};
