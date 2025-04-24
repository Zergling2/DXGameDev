#include <ZergEngine\CoreSystem\SceneTable.h>

using namespace ze;

std::map<PCSTR, SceneFactory, MultiByteStringComparator>* SceneTable::s_pSceneTable;

void SceneTable::AddItem(PCSTR sceneName, SceneFactory factory)
{
	if (SceneTable::s_pSceneTable == nullptr)
		SceneTable::s_pSceneTable = new std::map<PCSTR, SceneFactory, MultiByteStringComparator>();

	const auto result = SceneTable::s_pSceneTable->insert(std::make_pair(sceneName, factory));

	assert(result.second == true);
}

SceneFactory SceneTable::GetItem(PCSTR sceneName)
{
	assert(SceneTable::s_pSceneTable != nullptr);

	const auto result = SceneTable::s_pSceneTable->find(sceneName);

	if (result == SceneTable::s_pSceneTable->cend())
		return nullptr;
	else
		return result->second;
}
