#include "TestScene1.h"
#include "..\Script\FirstPersonMovement.h"
#include "..\Script\Planet.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(TestScene1)

void TestScene1::OnLoadScene()
{
	Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"Resource\\Skybox\\indigo.dds", false);
	RenderSettings::GetInstance()->SetSkybox(skyboxCubeMap);
}
