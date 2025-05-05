#include "SceneChange.h"

using namespace ze;
using namespace pubg;

void SceneChange::Update()
{
	if (Input.GetKeyDown(KEY_NUMPAD1))
		SceneManager.LoadScene("TestScene1");

	if (Input.GetKeyDown(KEY_NUMPAD2))
		SceneManager.LoadScene("TestScene2");

	if (Input.GetKeyDown(KEY_NUMPAD3))
		SceneManager.LoadScene("SpaceTestScene");
}
