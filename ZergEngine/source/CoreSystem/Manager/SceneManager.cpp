#include <ZergEngine\CoreSystem\Manager\SceneManager.h>
#include <ZergEngine\CoreSystem\SceneInterface.h>
#include <ZergEngine\CoreSystem\Debug.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\UIObjectInterface.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

using namespace ze;

SceneManager* SceneManager::s_pInstance = nullptr;

SceneManager::SceneManager()
	: m_lock()
	, m_pNextScene(nullptr)
{
	m_lock.Init();
}

SceneManager::~SceneManager()
{
}

void SceneManager::CreateInstance()
{
	assert(s_pInstance == nullptr);

	s_pInstance = new SceneManager();
}

void SceneManager::DestroyInstance()
{
	assert(s_pInstance != nullptr);

	delete s_pInstance;
	s_pInstance = nullptr;
}

void SceneManager::Init(PCWSTR startScene)
{
	this->LoadScene(startScene);
}

void SceneManager::UnInit()
{
	// safe cleanup...
	Helper::SafeDelete(m_pNextScene);
}

bool SceneManager::LoadScene(PCWSTR sceneName)
{
	bool success = false;

	do
	{
		if (sceneName == nullptr)
			break;

		if (m_pNextScene != nullptr)		// �̹� ������� ���� �ִ� ���
			break;

		m_pNextScene = this->CreateScene(sceneName);
		if (m_pNextScene == nullptr)		// �������� �ʴ� ���� ���
			break;

		m_pNextScene->OnLoadScene();

		success = true;
	} while (false);

	return success;
}

IScene* SceneManager::CreateScene(PCWSTR sceneName)
{
	IScene* pNewScene = nullptr;

	SceneFactory sf = SceneTable::GetItem(sceneName);
	if (sf == nullptr)
		Debug::ForceCrashWithMessageBox(L"Error", L"The scene name '%s' does not exist!");
	else
		pNewScene = sf();

	return pNewScene;
}

IScene* SceneManager::PopNextScene()
{
	if (!m_pNextScene)
		return nullptr;

	AutoAcquireSlimRWLockExclusive autolock(m_lock);
	IScene* pTop = m_pNextScene;

	m_pNextScene = nullptr;

	return pTop;
}
