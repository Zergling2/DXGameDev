#include "TestScript.h"

using namespace pubg;
using namespace ze;

void TestScript::FixedUpdate()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();

	if (!pGameObject)
		return;

	XMFLOAT3A position = pGameObject->GetTransform().m_position;
	position.z += 0.01f;
	pGameObject->GetTransform().m_position = position;

	XMFLOAT4A rotation = pGameObject->GetTransform().m_rotation;
	rotation.y += 0.005f;
	pGameObject->GetTransform().m_rotation = rotation;
	
	ComponentHandle hMeshRenderer = pGameObject->GetComponent<ze::MeshRenderer>();
	MeshRenderer* pMeshRenderer = static_cast<MeshRenderer*>(hMeshRenderer.ToPtr());
	if (!pMeshRenderer)
		return;

	static int i = 0;
	i++;

	static std::shared_ptr<ze::Texture2D> tex;
	if (i == 60)
	{
		if (pMeshRenderer->m_mesh->m_subsets[0].m_material->m_baseMap)
		{
			tex = pMeshRenderer->m_mesh->m_subsets[0].m_material->m_baseMap;
			pMeshRenderer->m_mesh->m_subsets[0].m_material->m_baseMap = nullptr;
		}
		else
		{
			pMeshRenderer->m_mesh->m_subsets[0].m_material->m_baseMap = tex;
			tex = nullptr;
		}

		i = 0;
	}
}
