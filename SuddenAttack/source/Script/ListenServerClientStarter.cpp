#include "ListenServerClientStarter.h"
#include "ListenServerClient.h"
#include "..\Resource\GlobalScriptGameObject.h"

using namespace ze;

ListenServerClientStarter::ListenServerClientStarter(ze::GameObject& owner)
	: MonoBehaviour(owner)
{
}

void ListenServerClientStarter::Start()
{
	// 씬에 배치되고 시작될 때 리슨 서버 깨우고 자기 자신은 파괴 요청
	GameObjectHandle hGameObjGlobalScripts = GameObject::Find(GLOBAL_SCRIPTS_GAME_OBJECT_NAME);
	assert(hGameObjGlobalScripts.IsValid());
	GameObject* pGameObjGlobalScripts = hGameObjGlobalScripts.ToPtr();

	ComponentHandle<ListenServerClient> hScriptListenServerClient = pGameObjGlobalScripts->GetComponent<ListenServerClient>();
	ListenServerClient* pScriptListenServerClient = hScriptListenServerClient.ToPtr();

	pScriptListenServerClient->StartClient();

	this->m_pGameObject->Destroy();
}
