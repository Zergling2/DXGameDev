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
		// OnLoadScene() 함수에서는 반드시 RuntimeImpl::CreateGameObject() 함수 대신 이 함수로 게임 오브젝트를 생성해야 합니다.
		// 이 함수가 반환한 객체 포인터에 절대로 delete를 호출해서는 안됩니다. OnLoadScene() 함수에서는 객체 생성만이 가능합니다.
		// 명시적 객체 파괴는 스크립트에서 핸들을 통해서만 가능하며, 암시적 객체 파괴는 씬 전환 시
		// DontDestroyOnLoad() 함수로 지정되지 않은 객체들에 한해서 자동으로 이루어집니다.
		GameObject* CreateGameObject(PCWSTR name = L"New Game Object");
	private:
		// OnLoadScene에서는 반드시 RuntimeImpl::CreateGameObject() 함수 대신 IScene::CreateGameObject() 함수를 사용하여 게임 오브젝트를 생성해야 합니다.
		virtual void OnLoadScene() = 0;
	private:
		std::vector<GameObject*>* m_pDeferredGameObjects;
	};
}
