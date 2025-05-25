#include <ZergEngine\CoreSystem\SceneInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>

using namespace ze;

IScene::IScene()
	: m_upDeferredGameObjects(std::make_unique<std::vector<GameObject*>>())
	, m_upDeferredUIObjects(std::make_unique<std::vector<IUIObject*>>())
{
}

GameObjectHandle IScene::CreateGameObject(PCWSTR name)
{
	// Deferred game object.
	GameObject* pGameObject = new GameObject(
		static_cast<GAMEOBJECT_FLAG>(static_cast<uint16_t>(GAMEOBJECT_FLAG::DEFERRED) | static_cast<uint16_t>(GAMEOBJECT_FLAG::ACTIVE)),
		name
	);

	GameObjectHandle hGameObject = GameObjectManager.RegisterToHandleTable(pGameObject);
	m_upDeferredGameObjects->push_back(pGameObject);

	return hGameObject;
}

UIObjectHandle IScene::CreateButton(PCWSTR name)
{
	// Deferred UI object.
	Button* pButton = new Button(
		static_cast<UIOBJECT_FLAG>(static_cast<uint16_t>(UIOBJECT_FLAG::DEFERRED) | static_cast<uint16_t>(UIOBJECT_FLAG::ACTIVE)),
		name
	);

	UIObjectHandle hButton = UIObjectManager.RegisterToHandleTable(pButton);
	m_upDeferredUIObjects->push_back(pButton);

	return hButton;
}
