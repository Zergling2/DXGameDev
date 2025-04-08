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

	// ������ ���ӿ�����Ʈ�� �� ������Ʈ���� ���� ����
	// ���۵� ��� ��ȸ�ϸ鼭 deferred �÷��� ��� ����������Ѵ�.
	// �׸��� �������� �ϸ鼭 �ڵ��� �޴µ�
	for (auto pGameObject : m_upNextScene->m_pDeferredGameObjects)
	{
		GameObjectHandle hGameObject = GameObjectManager.Register(pGameObject);
		if (!hGameObject.IsValid())
			Debug::ForceCrashWithMessageBox(
				L"Move deferred game objects",
				L"Failed to create game object. name: '%s'\n", pGameObject->GetName()
			);

		for (auto& hComponentFakeHandle : pGameObject->m_components)	// ��¥ �ڵ��� ����
		{
			IComponent* pComponent = hComponentFakeHandle.m_pComponent;
			pComponent->m_hGameObject = hGameObject;	// ������Ʈ���� �ڽ��� �����ϴ� ������Ʈ�� ��¥ �ڵ��� �˷��ش�.

			IComponentManager* pComponentManager = pComponent->GetComponentManager();
			hComponentFakeHandle = pComponentManager->Register(pComponent);	// ��¥ �ڵ��� ��¥ �ڵ�� ������Ʈ
			if (!hComponentFakeHandle.IsValid())
				Debug::ForceCrashWithMessageBox(
					L"Move deferred game objects",
					L"Failed to create %s's component.\n", pGameObject->GetName()
				);
		}
	}

	m_upNextScene->m_pDeferredGameObjects.clear();

	// ���� �� ���� �� ���� ������ ������ ��ü
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
