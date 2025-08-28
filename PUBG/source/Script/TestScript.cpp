#include "TestScript.h"

using namespace pubg;
using namespace ze;

Texture2D tex;

void TestScript::FixedUpdate()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();

	if (!pGameObject)
		return;

	pGameObject->m_transform.Translate(XMVectorMultiply(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), XMVectorReplicate(Time::GetInstance()->GetDeltaTime())));

	pGameObject->m_transform.Rotate(XMQuaternionRotationAxis(Math::Vector3::Up(), XMConvertToRadians(90) * Time::GetInstance()->GetDeltaTime()));
	
	ComponentHandle<MeshRenderer> hMeshRenderer = pGameObject->GetComponent<MeshRenderer>();
	MeshRenderer* pMeshRenderer = hMeshRenderer.ToPtr();
	if (!pMeshRenderer)
		return;

	static int i = 0;
	i++;

	if (i == 60)
	{
		if (pMeshRenderer->GetMaterialPtr(0)->m_diffuseMap)
		{
			tex = pMeshRenderer->GetMaterialPtr(0)->m_diffuseMap;
			pMeshRenderer->GetMaterialPtr(0)->m_diffuseMap.Reset();
		}
		else
		{
			pMeshRenderer->GetMaterialPtr(0)->m_diffuseMap = tex;
			tex.Reset();
		}

		i = 0;
	}
}

void TestScript::OnDestroy()
{
	tex.Reset();
}
