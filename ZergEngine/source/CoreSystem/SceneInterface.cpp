#include <ZergEngine\CoreSystem\SceneInterface.h>
#include <ZergEngine\CoreSystem\Manager\GameObjectManager.h>
#include <ZergEngine\CoreSystem\Manager\UIObjectManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Panel.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Button.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Text.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\InputField.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\SliderControl.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Checkbox.h>
#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\RadioButton.h>

using namespace ze;

IScene::IScene()
	: m_pendingGameObjects()
	, m_pendingUIObjects()
{
}

GameObjectHandle IScene::CreateGameObject(PCWSTR name)
{
	GameObject* pGameObject = nullptr;
	GameObjectHandle hGameObject = GameObjectManager::GetInstance()->CreatePendingObject(&pGameObject, name);
	m_pendingGameObjects.push_back(pGameObject);

	return hGameObject;
}

UIObjectHandle IScene::CreatePanel(PCWSTR name)
{
	Panel* pPanel = nullptr;
	UIObjectHandle hPanel = UIObjectManager::GetInstance()->CreatePendingObject<Panel>(&pPanel, name);
	m_pendingUIObjects.push_back(pPanel);

	return hPanel;
}

UIObjectHandle IScene::CreateImage(PCWSTR name)
{
	Image* pImage = nullptr;
	UIObjectHandle hImage = UIObjectManager::GetInstance()->CreatePendingObject<Image>(&pImage, name);
	m_pendingUIObjects.push_back(pImage);

	return hImage;
}

UIObjectHandle IScene::CreateText(PCWSTR name)
{
	Text* pText = nullptr;
	UIObjectHandle hText = UIObjectManager::GetInstance()->CreatePendingObject<Text>(&pText, name);
	m_pendingUIObjects.push_back(pText);

	return hText;
}

UIObjectHandle IScene::CreateButton(PCWSTR name)
{
	Button* pButton = nullptr;
	UIObjectHandle hButton = UIObjectManager::GetInstance()->CreatePendingObject<Button>(&pButton, name);
	m_pendingUIObjects.push_back(pButton);

	return hButton;
}

UIObjectHandle IScene::CreateInputField(PCWSTR name)
{
	InputField* pInputField = nullptr;
	UIObjectHandle hInputField = UIObjectManager::GetInstance()->CreatePendingObject<InputField>(&pInputField, name);
	m_pendingUIObjects.push_back(pInputField);

	return hInputField;
}

UIObjectHandle IScene::CreateSliderControl(PCWSTR name)
{
	SliderControl* pSliderControl = nullptr;
	UIObjectHandle hSliderControl = UIObjectManager::GetInstance()->CreatePendingObject<SliderControl>(&pSliderControl, name);
	m_pendingUIObjects.push_back(pSliderControl);

	return hSliderControl;
}

UIObjectHandle IScene::CreateCheckbox(PCWSTR name)
{
	Checkbox* pCheckbox = nullptr;
	UIObjectHandle hCheckbox = UIObjectManager::GetInstance()->CreatePendingObject<Checkbox>(&pCheckbox, name);
	m_pendingUIObjects.push_back(pCheckbox);

	return hCheckbox;
}

UIObjectHandle IScene::CreateRadioButton(PCWSTR name)
{
	RadioButton* pRadioButton = nullptr;
	UIObjectHandle hRadioButton = UIObjectManager::GetInstance()->CreatePendingObject<RadioButton>(&pRadioButton, name);
	m_pendingUIObjects.push_back(pRadioButton);

	return hRadioButton;
}
