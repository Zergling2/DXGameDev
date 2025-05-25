#pragma once

#include <ZergEngine\CoreSystem\SceneTable.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	// Scene base class.
	class IScene abstract
	{
		friend class SceneManagerImpl;
	public:
		IScene();
		virtual ~IScene() = default;
	protected:
		// OnLoadScene() �Լ������� �ݵ�� RuntimeImpl::CreateGameObject() �Լ� ��� �ݵ�� �� �Լ��� ���� ������Ʈ�� �����ؾ� �մϴ�.
		// OnLoadScene() �Լ������� GameObject �� Component �ı��� ������ ������ �������� �����մϴ�.
		// ����� ��ü �ı��� ��ũ��Ʈ���� �ڵ��� ���ؼ��� �����ϸ�, �Ͻ��� ��ü �ı��� �� ��ȯ ��
		// DontDestroyOnLoad() �Լ��� �������� ���� ��ü�鿡 ���ؼ� �ڵ����� �̷�����ϴ�.
		GameObjectHandle CreateGameObject(PCWSTR name = L"New Game Object");

		// ��Ʈ ������Ʈ�� ��ư�� �����մϴ�.
		UIObjectHandle CreateButton(PCWSTR name = L"New Button");
	private:
		// OnLoadScene �Լ��� ������ �ݵ�� Runtime�� Create �Լ����� ������� ���� 
		// IScene �������̽����� �����ϴ� Create �Լ����� ����Ͽ� ������Ʈ�� �����ؾ� �մϴ�.
		virtual void OnLoadScene() = 0;
	private:
		std::unique_ptr<std::vector<GameObject*>> m_upDeferredGameObjects;
		std::unique_ptr<std::vector<IUIObject*>> m_upDeferredUIObjects;
	};
}
