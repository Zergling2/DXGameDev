#include "TestScript.h"

using namespace pubg;
using namespace ze;

void TestScript::FixedUpdate()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();

	if (!pGameObject)
		return;

	ComponentHandle<Transform> hTransform = pGameObject->GetComponent<Transform>();
	Transform* pTransform = hTransform.ToPtr();

	pTransform->Translate(XMVectorMultiply(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMVectorReplicate(Time.GetDeltaTime())));

	pTransform->Rotate(XMQuaternionRotationAxis(WORLD_UP, XMConvertToRadians(90) * Time.GetDeltaTime()));
	
	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->GetComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	if (!pMeshRenderer)
		return;

	static int i = 0;
	i++;

	static Texture2D tex;
	if (i == 60)
	{
		if (pMeshRenderer->m_mesh->m_subsets[0].m_material->m_diffuseMap)
		{
			tex = pMeshRenderer->m_mesh->m_subsets[0].m_material->m_diffuseMap;
			pMeshRenderer->m_mesh->m_subsets[0].m_material->m_diffuseMap.Reset();
		}
		else
		{
			pMeshRenderer->m_mesh->m_subsets[0].m_material->m_diffuseMap = tex;
			tex.Reset();
		}

		i = 0;
	}
}
