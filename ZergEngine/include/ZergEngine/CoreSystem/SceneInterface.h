#pragma once

#include <ZergEngine\CoreSystem\SceneTable.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	// Scene base class.
	class IScene abstract
	{
		friend class Runtime;
		friend class SceneManager;
	public:
		IScene();
		virtual ~IScene() = default;
	protected:
		// OnLoadScene() �Լ������� �ݵ�� Runtime::CreateGameObject() �Լ� ��� �ݵ�� �� �Լ��� ���� ������Ʈ�� �����ؾ� �մϴ�.
		// OnLoadScene() �Լ������� GameObject �� Component �ı��� ������ ������ �������� �����մϴ�.
		// ����� ��ü �ı��� ��ũ��Ʈ���� �ڵ��� ���ؼ��� �����ϸ�, �Ͻ��� ��ü �ı��� �� ��ȯ ��
		// DontDestroyOnLoad() �Լ��� �������� ���� ��ü�鿡 ���ؼ� �ڵ����� �̷�����ϴ�.
		GameObjectHandle CreateGameObject(PCWSTR name = L"New Game Object");

		// ��Ʈ ������Ʈ�� �г� UI�� �����մϴ�.
		UIObjectHandle CreatePanel(PCWSTR name = L"New Panel");

		// ��Ʈ ������Ʈ�� �̹��� UI�� �����մϴ�.
		UIObjectHandle CreateImage(PCWSTR name = L"New Image");

		// ��Ʈ ������Ʈ�� �ؽ�Ʈ UI�� �����մϴ�.
		UIObjectHandle CreateText(PCWSTR name = L"New Text");

		// ��Ʈ ������Ʈ�� ��ư UI�� �����մϴ�.
		UIObjectHandle CreateButton(PCWSTR name = L"New Button");

		// ��Ʈ ������Ʈ�� �Է� �ʵ� UI�� �����մϴ�.
		UIObjectHandle CreateInputField(PCWSTR name = L"New Input Field");
	private:
		// OnLoadScene �Լ��� ������ �ݵ�� Runtime�� Create �Լ����� ������� ���� 
		// IScene �������̽����� �����ϴ� Create �Լ����� ����Ͽ� ������Ʈ�� �����ؾ� �մϴ�.
		virtual void OnLoadScene() = 0;
	private:
		std::vector<GameObject*> m_pendingGameObjects;
		std::vector<IUIObject*> m_pendingUIObjects;
	};
}
