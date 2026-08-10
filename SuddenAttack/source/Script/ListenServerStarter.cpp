#include "ListenServerStarter.h"
#include "ListenServer.h"
#include "..\Resource\GlobalScriptGameObject.h"

ListenServerStarter::ListenServerStarter(ze::GameObject& owner)
	: ze::MonoBehaviour(owner)
{
}

void ListenServerStarter::Start()
{
	// 씬에 배치되고 시작될 때 리슨 서버 깨우고 자기 자신은 파괴 요청
	ze::GameObjectHandle hGameObjGlobalScripts = ze::GameObject::Find(GLOBAL_SCRIPTS_GAME_OBJECT_NAME);
	assert(hGameObjGlobalScripts.IsValid());
	ze::GameObject* pGameObjGlobalScripts = hGameObjGlobalScripts.ToPtr();

	ze::ComponentHandle<ListenServer> hScriptListenServer = pGameObjGlobalScripts->GetComponent<ListenServer>();
	ListenServer* pScriptListenServer = hScriptListenServer.ToPtr();

	pScriptListenServer->StartServer();

	this->m_pGameObject->Destroy();
}
