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

		if (m_upNextScene != nullptr)		// �̹� ������� ���� �ִ� ���
			break;

		m_upNextScene = this->CreateScene(sceneName);
		if (m_upNextScene == nullptr)			// �������� �ʴ� ���� ���
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

	// �� ��ü �ʿ�
	// DontDestroyOnLoad ������Ʈ�� �����ϰ� ��� �ı�
	auto& activeGameObjects = GameObjectManager.m_activeGameObjects;
	for (GameObject* pGameObject : activeGameObjects)
		if (!pGameObject->IsDontDestroyOnLoad())
			Runtime.Destroy(pGameObject);

	Runtime.RemoveDestroyedComponentsAndGameObjects();

	// ������ ���ӿ�����Ʈ�� �� ������Ʈ���� ���� ����
	auto& deferredGameObjects = *m_upNextScene->m_pDeferredGameObjects;
	for (GameObject* pGameObject : deferredGameObjects)
	{
		GameObjectHandle hGameObject = GameObjectManager.Register(pGameObject);	// Deferred �÷��� ���ŵ� ����
		assert(hGameObject.IsValid() == true);

		for (IComponent* pComponent : pGameObject->m_components)
		{
			IComponentManager* pComponentManager = pComponent->GetComponentManager();
			pComponentManager->Register(pComponent);
		}
	}

	// �� �̻� �ʿ� ���Ե� ���� ���͸� ����
	delete m_upNextScene->m_pDeferredGameObjects;

	// ���� �� ���� �� ���� ������ ������ ��ü
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
