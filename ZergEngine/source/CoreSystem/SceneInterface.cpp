#include <ZergEngine\CoreSystem\SceneInterface.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>

using namespace ze;

IScene::IScene()
	: m_pendingGameObjects()
	, m_pendingUIObjects()
{
}

GameObjectHandle IScene::CreateGameObject(PCWSTR name)
{
	GameObject* pNewGameObject = nullptr;
	GameObjectHandle hNewGameObject = GameObjectManager::GetInstance()->CreatePendingObject(&pNewGameObject, name);
	m_pendingGameObjects.push_back(pNewGameObject);

	return hNewGameObject;
}

UIObjectHandle IScene::CreatePanel(PCWSTR name)
{
	Panel* pNewPanel = nullptr;
	UIObjectHandle hNewPanel = UIObjectManager::GetInstance()->CreatePendingObject<Panel>(&pNewPanel, name);
	m_pendingUIObjects.push_back(pNewPanel);

	return hNewPanel;
}

UIObjectHandle IScene::CreateImage(PCWSTR name)
{
	Image* pNewImage = nullptr;
	UIObjectHandle hNewImage = UIObjectManager::GetInstance()->CreatePendingObject<Image>(&pNewImage, name);
	m_pendingUIObjects.push_back(pNewImage);

	return hNewImage;
}

UIObjectHandle IScene::CreateButton(PCWSTR name)
{
	Button* pNewButton = nullptr;
	UIObjectHandle hNewButton = UIObjectManager::GetInstance()->CreatePendingObject<Button>(&pNewButton, name);
	m_pendingUIObjects.push_back(pNewButton);

	return hNewButton;
}
