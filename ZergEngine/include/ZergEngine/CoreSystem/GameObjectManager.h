#pragma once

#include <ZergEngine\CoreSystem\SubsystemInterface.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

namespace ze
{
	class GameObject;

	class GameObjectManager : public ISubsystem
	{
		friend class Runtime;
		friend class GameObject;
		friend class GameObjectHandle;
		friend class SceneManager;
		ZE_DECLARE_SINGLETON(GameObjectManager);
	private:
		virtual void Init(void* pDesc) override;
		virtual void Release() override;

		// deferred 플래그를 해제하고 게임오브젝트를 빈 공간에 넣고 m_index를 유효한 값을 대입해주고 유효한 실제 핸들을 반환해주어야 한다.
		GameObjectHandle Register(GameObject* pGameObject);

		// 해당 객체가 차지하는 공간을 nullptr로 만든다.
		// 만약 객체 파괴가 요청되었는데 해당 인덱스에 있는게 이 객체가 아니라면 뭔가 잘못된 상황이다. 핸들 기반 접근만 허용하기 때문에
		// 댕글링 포인터 문제도 없어야 한다. 제대로 정비해봐야 한다.
		// 메모리 해제는 해주지 않는다. 메모리 해제는 Runtime::Destroy() 함수에서 또는 SceneManager에서 오래된 씬이 파괴될 때 해준다.
		void Unregister(GameObject* pGameObject);

		uint64_t AssignUniqueId() { return InterlockedIncrement64(reinterpret_cast<LONG64*>(&m_uniqueId)); }
	private:
		uint64_t m_uniqueId;
		std::vector<GameObject*> m_activeGameObjects;	// 검색 시 전체 테이블을 순회할 필요를 없앤다.
		GameObject* m_ptrTable[64 * 128];
	};
}
