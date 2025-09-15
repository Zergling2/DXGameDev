#include <ZergEngine\CoreSystem\SceneTable.h>

using namespace ze;

std::map<PCWSTR, SceneFactory, WideCharStringComparator>* SceneTable::s_pSceneTable;

void SceneTable::AddItem(PCWSTR sceneName, SceneFactory factory)
{
	if (SceneTable::s_pSceneTable == nullptr)
		SceneTable::s_pSceneTable = new std::map<PCWSTR, SceneFactory, WideCharStringComparator>();

	const auto result = SceneTable::s_pSceneTable->insert(std::make_pair(sceneName, factory));

	assert(result.second == true);
}

SceneFactory SceneTable::GetItem(PCWSTR sceneName)
{
	assert(SceneTable::s_pSceneTable != nullptr);

	const auto result = SceneTable::s_pSceneTable->find(sceneName);

	if (result == SceneTable::s_pSceneTable->cend())
		return nullptr;
	else
		return result->second;
}

void SceneTable::DestroySceneTable()
{
	if (s_pSceneTable)
	{
		delete s_pSceneTable;

		s_pSceneTable = nullptr;
	}
}
