#include <ZergEngine\System\Scene.h>
#include <ZergEngine\System\SceneManager.h>
#include <ZergEngine\System\GameObject.h>
#include <ZergEngine\System\GameObjectManager.h>

using namespace zergengine;

IScene::IScene()
	: m_id(0)
	, m_deferredGameObjects()
	, m_skybox(nullptr)
	, m_terrain(nullptr)
{
}

IScene::~IScene()
{
}

std::shared_ptr<GameObject> IScene::CreateEmpty(const wchar_t* name)
{
	const bool isCurrentScene = SceneManager::GetCurrentSceneId() == m_id;

	std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(!isCurrentScene, false, name);

	if (isCurrentScene)
		GameObjectManager::AddGameObject(gameObject);
	else
		m_deferredGameObjects.emplace_back(gameObject);

	return gameObject;
}

void IScene::SetSkybox(const std::shared_ptr<Skybox>& skybox)
{
	m_skybox = skybox;
}

bool IScene::SetTerrain(const std::shared_ptr<Terrain>& terrain)
{
	// 씬 플레이 도중 지형 제거 기능은 아직 지원 X
	if (m_terrain != nullptr || terrain == nullptr)
		return false;

	m_terrain = terrain;

	// 지형 콜라이더 생성...

	return true;
}
