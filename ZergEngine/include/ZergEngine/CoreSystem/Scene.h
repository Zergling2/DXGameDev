#pragma once

#include <ZergEngine\CoreSystem\SceneTable.h>

namespace ze
{
	class GameObject;
	class Transform;

	// Scene base class.
	class IScene abstract
	{
		friend class SceneManagerImpl;
	public:
		IScene();
		virtual ~IScene() = default;
	protected:
		// OnLoadScene() �Լ������� �ݵ�� RuntimeImpl::CreateGameObject() �Լ� ��� �� �Լ��� ���� ������Ʈ�� �����ؾ� �մϴ�.
		// �� �Լ��� ��ȯ�� ��ü �����Ϳ� ����� delete�� ȣ���ؼ��� �ȵ˴ϴ�. OnLoadScene() �Լ������� ��ü �������� �����մϴ�.
		// ����� ��ü �ı��� ��ũ��Ʈ���� �ڵ��� ���ؼ��� �����ϸ�, �Ͻ��� ��ü �ı��� �� ��ȯ ��
		// DontDestroyOnLoad() �Լ��� �������� ���� ��ü�鿡 ���ؼ� �ڵ����� �̷�����ϴ�.
		GameObject* CreateGameObject(PCWSTR name = L"New Game Object");
	private:
		// OnLoadScene������ �ݵ�� RuntimeImpl::CreateGameObject() �Լ� ��� IScene::CreateGameObject() �Լ��� ����Ͽ� ���� ������Ʈ�� �����ؾ� �մϴ�.
		virtual void OnLoadScene() = 0;
	private:
		std::vector<GameObject*>* m_pDeferredGameObjects;
	};
}
