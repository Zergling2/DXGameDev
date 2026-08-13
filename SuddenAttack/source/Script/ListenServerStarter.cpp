#include "ListenServerStarter.h"
#include "ListenServer.h"
#include "..\Resource\GlobalScriptGameObject.h"

using namespace ze;

ListenServerStarter::ListenServerStarter(ze::GameObject& owner)
	: MonoBehaviour(owner)
{
}

void ListenServerStarter::Start()
{
	// 씬에 배치되고 시작될 때 리슨 서버 깨우고 자기 자신은 파괴 요청
	GameObjectHandle hGameObjGlobalScripts = GameObject::Find(GLOBAL_SCRIPTS_GAME_OBJECT_NAME);
	assert(hGameObjGlobalScripts.IsValid());
	GameObject* pGameObjGlobalScripts = hGameObjGlobalScripts.ToPtr();

	ComponentHandle<ListenServer> hScriptListenServer = pGameObjGlobalScripts->GetComponent<ListenServer>();
	ListenServer* pScriptListenServer = hScriptListenServer.ToPtr();

	// 방장으로 시작해서 맵 씬을 로드한 경우에만 IsReady가 true를 반환한다.
	if (pScriptListenServer->IsReady())
		pScriptListenServer->StartServer();

	this->m_pGameObject->Destroy();
}
