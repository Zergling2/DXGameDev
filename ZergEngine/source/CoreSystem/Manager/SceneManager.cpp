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
	m_pNextScene = this->CreateScene(startScene);

	if (m_pNextScene == nullptr)
		Debug::ForceCrashWithMessageBox(L"Error", L"Can't find the start scene.");

	m_pNextScene->OnLoadScene();
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
		if (sceneName == nullptr || sceneName[0] == L'\0')
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
		wprintf(L"'%s' scene name does not exist!\n", sceneName);
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
