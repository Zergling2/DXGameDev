#include <ZergEngine\CoreSystem\Manager\SceneManager.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\CameraManager.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MonoBehaviour.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\SceneInterface.h>
#include <ZergEngine\CoreSystem\Runtime.h>

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
	, m_upNextScene(nullptr)
{
}

SceneManagerImpl::~SceneManagerImpl()
{
}

void SceneManagerImpl::Init(void* pDesc)
{
	m_lock.Init();

	std::unique_ptr<DummyScene> upCurrentScene = std::make_unique<DummyScene>();
	upCurrentScene->OnLoadScene();

	m_upNextScene = this->CreateScene(Runtime.GetStartSceneName());

	if (m_upNextScene == nullptr)
		Debug::ForceCrashWithMessageBox(L"Error", L"Can't find the start scene.");

	m_upNextScene->OnLoadScene();
}

void SceneManagerImpl::Release()
{
	// safe cleanup...
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

void SceneManagerImpl::Update(LONGLONG* pFixedUpdateTimer)
{
	if (m_upNextScene == nullptr)
		return;

	*pFixedUpdateTimer = 0;
	// 씬 교체

	// DontDestroyOnLoad 오브젝트를 제외하고 모두 파괴
	for (GameObject* pGameObject : GameObjectManager.m_activeGameObjects)
		if (!pGameObject->IsDontDestroyOnLoad())
			Runtime.Destroy(pGameObject);
	for (GameObject* pGameObject : GameObjectManager.m_inactiveGameObjects)
		if (!pGameObject->IsDontDestroyOnLoad())
			Runtime.Destroy(pGameObject);
	for (IUIObject* pUIObject : UIObjectManager.m_activeUIObjects)
		if (!pUIObject->IsDontDestroyOnLoad())
			Runtime.Destroy(pUIObject);
	for (IUIObject* pUIObject : UIObjectManager.m_inactiveUIObjects)
		if (!pUIObject->IsDontDestroyOnLoad())
			Runtime.Destroy(pUIObject);

	Runtime.RemoveDestroyedComponentsAndObjects();

	// 지연된 게임오브젝트들 및 컴포넌트들을 관리 시작
	std::vector<MonoBehaviour*> scripts;
	scripts.reserve(512);

	auto& deferredGameObjects = *m_upNextScene->m_upDeferredGameObjects;
	for (GameObject* pGameObject : deferredGameObjects)
	{
		// 중요 (플래그 제거)
		pGameObject->OffFlag(GAMEOBJECT_FLAG::DEFERRED);

		if (pGameObject->IsActive())
			GameObjectManager.AddPtrToActiveVector(pGameObject);
		else
			GameObjectManager.AddPtrToInactiveVector(pGameObject);

		for (IComponent* pComponent : pGameObject->m_components)
		{
			IComponentManager* pComponentManager = pComponent->GetComponentManager();
			pComponentManager->AddPtrToActiveVector(pComponent);

			if (pComponent->GetType() == COMPONENT_TYPE::MONOBEHAVIOUR)
				scripts.push_back(static_cast<MonoBehaviour*>(pComponent));
		}
	}

	auto& deferredUIObjects = *m_upNextScene->m_upDeferredUIObjects;
	for (IUIObject* pRootUIObject : deferredUIObjects)
	{
		// 루트 UI 오브젝트 처리
		// deferred ui 오브젝트들 벡터에는 루트 오브젝트들만 들어있다.
		assert(pRootUIObject->m_transform.m_pParentTransform == nullptr);
		pRootUIObject->OnFlag(UIOBJECT_FLAG::REAL_ROOT);
		UIObjectManager.AddPtrToRootVector(pRootUIObject);

		AddPtrRecursively(pRootUIObject);
	}

	// 게임오브젝트, UI오브젝트와 컴포넌트들이 모두 전역 관리자에 등록된 이후에 Awake, OnEnable, Start큐잉 처리
	for (MonoBehaviour* pScript : scripts)
	{
		pScript->Awake();
		if (pScript->IsEnabled())
		{
			pScript->OnEnable();
			MonoBehaviourManager.AddToStartingQueue(pScript);	// 활성화된 채로 씬에서 시작되는 경우 Start 대기열에 추가
		}
	}

	// 씬 로드 완료 후 필요없어진 씬 객체 제거
	m_upNextScene.reset();
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

void SceneManagerImpl::AddPtrRecursively(IUIObject* pUIObject)
{
	// 중요 (플래그 제거)
	pUIObject->OffFlag(UIOBJECT_FLAG::DEFERRED);

	if (pUIObject->IsActive())
		UIObjectManager.AddPtrToActiveVector(pUIObject);
	else
		UIObjectManager.AddPtrToInactiveVector(pUIObject);

	// 지연되어 있던 자식 UI 오브젝트들도 전역 관리 대상으로 추가
	for (RectTransform* pChildTransform : pUIObject->m_transform.m_children)
	{
		IUIObject* pChildUIObject = pChildTransform->m_pUIObject;
		assert(pChildUIObject != nullptr);

		AddPtrRecursively(pChildUIObject);
	}
}
