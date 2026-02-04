#include "TestScene1.h"
#include "..\Script\FirstPersonMovement.h"

using namespace ze;

ZE_IMPLEMENT_SCENE(TestScene1)

void TestScene1::OnLoadScene()
{
	Texture2D skyboxCubeMap = ResourceLoader::GetInstance()->LoadTexture2D(L"resources\\skybox\\indigo.dds", false);
	RenderSettings::GetInstance()->SetSkybox(skyboxCubeMap);
}
