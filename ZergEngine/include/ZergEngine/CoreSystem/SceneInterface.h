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
		// OnLoadScene() 함수에서는 반드시 RuntimeImpl::CreateGameObject() 함수 대신 반드시 이 함수로 게임 오브젝트를 생성해야 합니다.
		// OnLoadScene() 함수에서는 GameObject 및 Component 파괴도 허용되지 않으며 생성만이 가능합니다.
		// 명시적 객체 파괴는 스크립트에서 핸들을 통해서만 가능하며, 암시적 객체 파괴는 씬 전환 시
		// DontDestroyOnLoad() 함수로 지정되지 않은 객체들에 한해서 자동으로 이루어집니다.
		GameObjectHandle CreateGameObject(PCWSTR name = L"New Game Object");
	private:
		// OnLoadScene에서는 반드시 RuntimeImpl::CreateGameObject() 함수 대신 IScene::CreateGameObject() 함수를 사용하여 게임 오브젝트를 생성해야 합니다.
		virtual void OnLoadScene() = 0;
	private:
		std::vector<GameObject*>* m_pDeferredGameObjects;
	};
}
