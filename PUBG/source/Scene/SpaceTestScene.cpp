#include "SpaceTestScene.h"
#include "..\Script\FirstPersonCamera.h"

using namespace ze;
using namespace pubg;

ZE_IMPLEMENT_SCENE(SpaceTestScene);

void SpaceTestScene::OnLoadScene()
{
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\cloudy_puresky.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\warm_restaurant_night.dds");
	// Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\snowcube.dds");
	Texture2D skyboxCubeMap = Resource.LoadCubeMapTexture(L"Resource\\Skybox\\sunsetcube.dds");
	Environment.SetSkyboxCubeMap(skyboxCubeMap);
}
