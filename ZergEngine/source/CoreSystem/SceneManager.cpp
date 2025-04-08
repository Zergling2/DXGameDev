#include <ZergEngine\CoreSystem\SceneManager.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\Scene.h>
#include <ZergEngine\CoreSystem\SceneTable.h>
#include <ZergEngine\CoreSystem\Runtime.h>
#include <ZergEngine\CoreSystem\GameObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\ComponentSystem\ComponentManagerInterface.h>

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
	m_upNextScene->OnLoadScene();
}

void SceneManagerImpl::Release()
{
	// safe cleanup...

	m_upCurrentScene.reset();
	m_upNextScene.reset();
}

bool SceneManagerImpl::LoadScene(PCWSTR sceneName)
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
	size_t cursor = 0;
	while (cursor < activeGameObjects.size())
	{
		GameObject* pGameObject = activeGameObjects[cursor];

		if (pGameObject->IsDontDestroyOnLoad())
		{
			++cursor;
		}
		else
		{
			GameObjectHandle hGameObject = pGameObject->ToHandle();
			assert(hGameObject.IsValid());
			Runtime.Destroy(hGameObject);
		}
	}

	// 지연된 게임오브젝트들 및 컴포넌트들을 관리 시작
	// 디퍼드 목록 순회하면서 deferred 플래그 모두 제거해줘야한다.
	// 그리고 레지스터 하면서 핸들을 받는데
	for (auto pGameObject : m_upNextScene->m_pDeferredGameObjects)
	{
		GameObjectHandle hGameObject = GameObjectManager.Register(pGameObject);
		if (!hGameObject.IsValid())
			Debug::ForceCrashWithMessageBox(
				L"Move deferred game objects",
				L"Failed to create game object. name: '%s'\n", pGameObject->GetName()
			);

		for (auto& hComponentFakeHandle : pGameObject->m_components)	// 가짜 핸들의 참조
		{
			IComponent* pComponent = hComponentFakeHandle.m_pComponent;
			pComponent->m_hGameObject = hGameObject;	// 컴포넌트에게 자신을 소유하는 오브젝트의 진짜 핸들을 알려준다.

			IComponentManager* pComponentManager = pComponent->GetComponentManager();
			hComponentFakeHandle = pComponentManager->Register(pComponent);	// 가짜 핸들을 진짜 핸들로 업데이트
			if (!hComponentFakeHandle.IsValid())
				Debug::ForceCrashWithMessageBox(
					L"Move deferred game objects",
					L"Failed to create %s's component.\n", pGameObject->GetName()
				);
		}
	}

	m_upNextScene->m_pDeferredGameObjects.clear();

	// 기존 씬 제거 및 다음 씬으로 포인터 교체
	m_upCurrentScene = std::move(m_upNextScene);
}

std::unique_ptr<IScene> SceneManagerImpl::CreateScene(PCWSTR sceneName)
{
	std::unique_ptr<IScene> upNewScene;

	const SceneFactory sf = SceneTable::GetItem(sceneName);
	if (sf == nullptr)
		GlobalLog.GetSyncFileLogger().WriteFormat(L"'%s' scene name does not exist!\n", sceneName);
	else
		upNewScene = sf();

	return upNewScene;
}
