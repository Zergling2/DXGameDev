#include "SceneChange.h"

using namespace ze;
using namespace pubg;

void SceneChange::Update()
{
	if (Input::GetInstance()->GetKeyDown(KEY_NUMPAD1))
		SceneManager::GetInstance()->LoadScene(L"TestScene1");

	if (Input::GetInstance()->GetKeyDown(KEY_NUMPAD2))
		SceneManager::GetInstance()->LoadScene(L"TestScene2");

	if (Input::GetInstance()->GetKeyDown(KEY_NUMPAD3))
		SceneManager::GetInstance()->LoadScene(L"SpaceTestScene");
}
