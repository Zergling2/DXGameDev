#include "TestScript.h"

using namespace pubg;

void TestScript::FixedUpdate()
{
	XMFLOAT3A position = this->GetGameObject()->GetTransform().m_position;
	position.z += 0.01f;
	this->GetGameObject()->GetTransform().m_position = position;

	XMFLOAT4A rotation = this->GetGameObject()->GetTransform().m_rotation;
	rotation.y += 0.005f;
	this->GetGameObject()->GetTransform().m_rotation = rotation;
	
	std::shared_ptr<zergengine::MeshRenderer> mr = this->GetGameObject()->GetComponent<zergengine::MeshRenderer>();

	static int i = 0;
	i++;

	static std::shared_ptr<zergengine::Texture2D> tex;
	if (i == 60)
	{
		if (mr->m_mesh->m_subsets[0].m_material->m_baseMap)
		{
			tex = mr->m_mesh->m_subsets[0].m_material->m_baseMap;
			mr->m_mesh->m_subsets[0].m_material->m_baseMap = nullptr;
		}
		else
		{
			mr->m_mesh->m_subsets[0].m_material->m_baseMap = tex;
			tex = nullptr;
		}

		i = 0;
	}
}
