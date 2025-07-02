#include "MovePointLight.h"

using namespace pubg;
using namespace ze;

void MovePointLight::Update()
{
	GameObject* pGameObject = this->GetGameObjectHandle().ToPtr();
	if (!pGameObject)
		return;

	ComponentHandle<PointLight> hPointLight = pGameObject->GetComponent<PointLight>();
	PointLight* pPointLight = hPointLight.ToPtr();
	XMFLOAT3 att = pPointLight->GetAtt();
	if (Input::GetInstance()->GetKey(KEY_5))
		att.x -= 0.01f;
	if (Input::GetInstance()->GetKey(KEY_6))
		att.x += 0.01f;
	if (Input::GetInstance()->GetKey(KEY_7))
		att.y -= 0.01f;
	if (Input::GetInstance()->GetKey(KEY_8))
		att.y += 0.01f;
	if (Input::GetInstance()->GetKey(KEY_9))
		att.z -= 0.01f;
	if (Input::GetInstance()->GetKey(KEY_0))
		att.z += 0.01f;
	pPointLight->SetAtt(&att);

	XMFLOAT3A translation;
	XMStoreFloat3A(&translation, XMVectorZero());

	if (Input::GetInstance()->GetKey(KEY_NUMPAD4))
		translation.x -= Time::GetInstance()->GetDeltaTime();

	if (Input::GetInstance()->GetKey(KEY_NUMPAD6))
		translation.x += Time::GetInstance()->GetDeltaTime();

	if (Input::GetInstance()->GetKey(KEY_NUMPAD8))
		translation.z += Time::GetInstance()->GetDeltaTime();

	if (Input::GetInstance()->GetKey(KEY_NUMPAD2))
		translation.z -= Time::GetInstance()->GetDeltaTime();

	if (Input::GetInstance()->GetKey(KEY_NUMPAD7))
		translation.y -= Time::GetInstance()->GetDeltaTime();

	if (Input::GetInstance()->GetKey(KEY_NUMPAD9))
		translation.y += Time::GetInstance()->GetDeltaTime();

	pGameObject->m_transform.Translate(XMLoadFloat3A(&translation));
}
