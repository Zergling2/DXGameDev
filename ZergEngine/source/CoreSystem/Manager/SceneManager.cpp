#include <ZergEngine\CoreSystem\Manager\SceneManager.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Scene.h>
#include <ZergEngine\CoreSystem\SceneTable.h>
#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ComponentInterface.h>

namespace ze
{
	SceneManagerImpl SceneManager;
}

using namespace ze;

class DummyScene : public IScene
{
public:
	DummyScene() = default;
	virtual ~DummyScene() = default;
	virtual void OnLoadScene() override {}
};

SceneManagerImpl::SceneManagerImpl()
	: m_lock()
	, m_upCurrentScene(nullptr)
	, m_upNextScene(nullptr)
{
}

SceneManagerImpl::~SceneManagerImpl()
{
}

void SceneManagerImpl::Init(void* pDesc)
{
	m_lock.Init();

	m_upCurrentScene = std::make_unique<DummyScene>();
	m_upCurrentScene->OnLoadScene();

	m_upNextScene = this->CreateScene(Runtime.GetStartSceneName());

	if (m_upNextScene == nullptr)
		Debug::ForceCrashWithMessageBox(L"Error", L"Can't find the start scene.");

	m_upNextScene->OnLoadScene();
}

void SceneManagerImpl::Release()
{
	// safe cleanup...

	m_upCurrentScene.reset();
	m_upNextScene.reset();
}

bool SceneManagerImpl::LoadScene(PCSTR sceneName)
{
	bool success = false;

	do
	{
		if (sceneName == nullptr || sceneName[0] == L'\0')
			break;

		if (m_upNextScene != nullptr)		// 이미 대기중인 씬이 있는 경우
			break;

		m_upNextScene = this->CreateScene(sceneName);
		if (m_upNextScene == nullptr)			// 존재하지 않는 씬일 경우
			break;

		m_upNextScene->OnLoadScene();

		success = true;
	} while (false);

	return success;
}

void SceneManagerImpl::Update(float* pFixedUpdateTimer)
{
	if (m_upNextScene == nullptr)
		return;

	*pFixedUpdateTimer = 0.0f;

	// 씬 교체 필요
	// DontDestroyOnLoad 오브젝트를 제외하고 모두 파괴
	auto& activeGameObjects = GameObjectManager.m_activeGameObjects;
	for (GameObject* pGameObject : activeGameObjects)
		if (!pGameObject->IsDontDestroyOnLoad())
			Runtime.Destroy(pGameObject);

	Runtime.RemoveDestroyedComponentsAndGameObjects();

	// 지연된 게임오브젝트들 및 컴포넌트들을 관리 시작
	auto& deferredGameObjects = *m_upNextScene->m_pDeferredGameObjects;
	for (GameObject* pGameObject : deferredGameObjects)
	{
		GameObjectHandle hGameObject = GameObjectManager.Register(pGameObject);	// Deferred 플래그 제거도 포함
		assert(hGameObject.IsValid() == true);

		for (IComponent* pComponent : pGameObject->m_components)
		{
			IComponentManager* pComponentManager = pComponent->GetComponentManager();
			pComponentManager->Register(pComponent);
		}
	}

	// 더 이상 필요 없게된 지연 벡터를 해제
	delete m_upNextScene->m_pDeferredGameObjects;

	// 기존 씬 제거 및 다음 씬으로 포인터 교체
	m_upCurrentScene = std::move(m_upNextScene);
}

std::unique_ptr<IScene> SceneManagerImpl::CreateScene(PCSTR sceneName)
{
	std::unique_ptr<IScene> upNewScene;

	const SceneFactory sf = SceneTable::GetItem(sceneName);
	if (sf == nullptr)
		printf("'%s' scene name does not exist!\n", sceneName);
	else
		upNewScene = sf();

	return upNewScene;
}
