#include <ZergEngine\System\SceneManager.h>
#include <ZergEngine\System\Runtime.h>
#include <ZergEngine\System\Scene.h>
#include <ZergEngine\System\GameObject.h>
#include <ZergEngine\System\GameObjectManager.h>
#include <ZergEngine\System\Debug.h>
#include <ZergEngine\System\ComponentSystem.h>
#include <ZergEngine\System\Component\IScript.h>
#include <ZergEngine\System\Resource.h>

using namespace zergengine;

const SceneTableMap* SceneManager::s_pStm = nullptr;
sceneId SceneManager::s_sceneIdForIssue = 0;
IScene* SceneManager::s_pCurrentScene = nullptr;
IScene* SceneManager::s_pNextScene = nullptr;
SRWLOCK SceneManager::s_lock;

class DummyScene : public IScene
{
public:
	virtual ~DummyScene()
	{
	}
private:
	virtual void LoadScene() override{ /* Do nothing. */ }
};

sysresult SceneManager::SysStartup(const EngineSystemInitDescriptor& desc)
{
	InitializeSRWLock(&SceneManager::s_lock);

	// initialize...
	SceneManager::s_pStm = &desc.stm;

	SceneManager::s_pCurrentScene = new DummyScene();		// HandleChangeScene에서 분기 제거용
	SceneManager::s_pNextScene = SceneManager::CreateScene(Engine::GetStartSceneName());
	SceneManager::s_pNextScene->LoadScene();

	return 0;
}

void SceneManager::SysCleanup()
{
	// safe cleanup...

	SafeDelete(SceneManager::s_pCurrentScene);
	SafeDelete(SceneManager::s_pNextScene);
}

sceneId SceneManager::GetCurrentSceneId()
{
	return SceneManager::s_pCurrentScene->GetId();
}

bool SceneManager::LoadScene(const wchar_t* sceneName)
{
	if (SceneManager::s_pNextScene)		// 이미 대기중인 씬이 있는 경우
		return false;

	IScene* pNewScene = SceneManager::CreateScene(Engine::GetStartSceneName());
	if (!pNewScene)
		return false;

	pNewScene->LoadScene();
	SceneManager::s_pNextScene = pNewScene;

	return true;
}

bool SceneManager::HandleSceneChange()
{
	if (!SceneManager::s_pNextScene)
		return false;

	// DontDestroyOnLoad 오브젝트를 제외하고 모두 파괴
	const auto end = GameObjectManager::s_gameObjects.cend();
	auto iter = GameObjectManager::s_gameObjects.begin();
	while (iter != end)
	{
		GameObject* const pGameObject = iter->first;
		if (pGameObject->IsDestroyed() == false && pGameObject->IsDontDestroyOnLoad())
		{
			++iter;
		}
		else
		{
			pGameObject->m_destroyed = true;
			pGameObject->DestroyAllComponents();
			iter = GameObjectManager::s_gameObjects.erase(iter);
		}
	}

	// 지연된 게임오브젝트들 및 컴포넌트들을 관리 시작
	for (auto& go : SceneManager::s_pNextScene->m_deferredGameObjects)
	{
		go->m_isDeferred = false;
		GameObjectManager::AddGameObject(go);
		for (auto& component : go->m_components)
		{
			assert(component->IsDestroyed() == false);
			ComponentSystem::AddComponentToSystem(component);
		}
	}
	SceneManager::s_pNextScene->m_deferredGameObjects.clear();

	// 기존 씬 제거 및 다음 씬으로 포인터 교체
	delete SceneManager::s_pCurrentScene;
	SceneManager::s_pCurrentScene = SceneManager::s_pNextScene;
	SceneManager::s_pNextScene = nullptr;

	return true;
}

IScene* SceneManager::CreateScene(const wchar_t* sceneName)
{
	IScene* pNewScene = SceneManager::s_pStm->find(Engine::GetStartSceneName())->second();
	if (!pNewScene)
	{
		LogBuffer* pLogBuffer = AsyncFileLogger::GetLogBuffer();
		StringCbPrintfW(pLogBuffer->buffer, LogBuffer::BUFFER_SIZE, L"'%s' The scene name does not exist!\n", sceneName);
		AsyncFileLogger::PushLog(pLogBuffer);
		return nullptr;
	}
	
	pNewScene->m_id = InterlockedIncrement64(&SceneManager::s_sceneIdForIssue);
	return pNewScene;
}
