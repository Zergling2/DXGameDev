#include <ZergEngine\CoreSystem\SceneInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>
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

UIObjectHandle IScene::CreatePanel(PCWSTR name)
{
	// Deferred UI object.
	Panel* pPanel = new Panel(
		static_cast<UIOBJECT_FLAG>(static_cast<uint16_t>(UIOBJECT_FLAG::DEFERRED) | static_cast<uint16_t>(UIOBJECT_FLAG::ACTIVE)),
		name
	);

	return this->AddDeferredRootUIObject(pPanel);
}

UIObjectHandle IScene::CreateImage(PCWSTR name)
{
	// Deferred UI object.
	Image* pImage = new Image(
		static_cast<UIOBJECT_FLAG>(static_cast<uint16_t>(UIOBJECT_FLAG::DEFERRED) | static_cast<uint16_t>(UIOBJECT_FLAG::ACTIVE)),
		name
	);

	return this->AddDeferredRootUIObject(pImage);
}

UIObjectHandle IScene::CreateButton(PCWSTR name)
{
	// Deferred UI object.
	Button* pButton = new Button(
		static_cast<UIOBJECT_FLAG>(static_cast<uint16_t>(UIOBJECT_FLAG::DEFERRED) | static_cast<uint16_t>(UIOBJECT_FLAG::ACTIVE)),
		name
	);

	return this->AddDeferredRootUIObject(pButton);
}

UIObjectHandle IScene::AddDeferredRootUIObject(IUIObject* pUIObject)
{
	UIObjectHandle hUIObject = UIObjectManager.RegisterToHandleTable(pUIObject);
	m_upDeferredUIObjects->push_back(pUIObject);

	// REAL_ROOT 플래그 활성화 및 Root Vector에 추가는 SceneManager에서 씬 로드 시 수행한다.

	return hUIObject;
}
