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
		// OnLoadScene() 함수에서는 반드시 Runtime::CreateGameObject() 함수 대신 반드시 이 함수로 게임 오브젝트를 생성해야 합니다.
		// OnLoadScene() 함수에서는 GameObject 및 Component 파괴도 허용되지 않으며 생성만이 가능합니다.
		// 명시적 객체 파괴는 스크립트에서 핸들을 통해서만 가능하며, 암시적 객체 파괴는 씬 전환 시
		// DontDestroyOnLoad() 함수로 지정되지 않은 객체들에 한해서 자동으로 이루어집니다.
		GameObjectHandle CreateGameObject(PCWSTR name = L"New Game Object");

		// 루트 오브젝트로 패널 UI를 생성합니다.
		UIObjectHandle CreatePanel(PCWSTR name = L"New Panel");

		// 루트 오브젝트로 이미지 UI를 생성합니다.
		UIObjectHandle CreateImage(PCWSTR name = L"New Image");

		// 루트 오브젝트로 텍스트 UI를 생성합니다.
		UIObjectHandle CreateText(PCWSTR name = L"New Text");

		// 루트 오브젝트로 버튼 UI를 생성합니다.
		UIObjectHandle CreateButton(PCWSTR name = L"New Button");

		// 루트 오브젝트로 입력 필드 UI를 생성합니다.
		UIObjectHandle CreateInputField(PCWSTR name = L"New Input Field");
	private:
		// OnLoadScene 함수의 구현은 반드시 Runtime의 Create 함수들을 사용하지 말고 
		// IScene 인터페이스에서 제공하는 Create 함수들을 사용하여 오브젝트를 생성해야 합니다.
		virtual void OnLoadScene() = 0;
	private:
		std::vector<GameObject*> m_pendingGameObjects;
		std::vector<IUIObject*> m_pendingUIObjects;
	};
}
