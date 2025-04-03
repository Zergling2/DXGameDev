#include <ZergEngine\CoreSystem\ComponentSystem\ScriptManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ScriptInterface.h>

using namespace ze;

ZE_IMPLEMENT_SINGLETON(ScriptManager);

ScriptManager::ScriptManager()
{
}

ScriptManager::~ScriptManager()
{
}

void ScriptManager::Init(void* pDesc)
{
}

void ScriptManager::Release()
{
}

void ScriptManager::FixedUpdateScripts()
{
    // 일단은 Deferred Remove를 이용.
    // 즉시 삭제 방법은 외부 이터레이터 손상을 막기 위해 인덱스로 접근한다던가 해야하고
    // 그렇게 하더라도 여러 예외처리가 들어가게 된다.
    
    // 물론 방법은 있다. 스크립트 매니저에 bool isUpdating과 현재 업데이트중인 스크립트의 인덱스를 적어두고
    // 만약 순회 중 즉시 삭제가 이루어지면 뒤에서 땡겨진 컴포넌트가 업데이트가 생략되는 일이 없게 해주는 식으로 Update를 해주고 앞쪽으로
    // 이동시킨다던지... 그러나 이런 식으로 하면 복잡도가 올라가게 된다.

    // 그리고 Deferred remove가 필요한 가장 중요한 이유
    // 스크립트에서 Destroy(gameObject) 또는 Destroy(this) 호출 후 return하지 않고 계속 접근하는 경우
    // Destroy 함수에서 지연 삭제를 요청해놓지 않고 바로 메모리 해제를 해버리면 잘못된 메모리 접근을 하게 된다.

    // 이런 경우도 있을 수 있다.
    // 컴포넌트가 Light나 Camera 이런거 멤버에 shared_ptr 두고 참조하고 있을 수 있는데
    // 컴포넌트가 얘네(light, camera같은 컴포넌트)를 Destroy하고 자기 자신을 Destroy 한다음에 그 밑에서 계속 light나 camera에 접근하는 코드를
    // 사용한다고 가정하면 만약 스크립트가 deferred destroy 방법으로 제거되지 않고 Destroy에서 바로 제거되었다면 스크립트 클래스의 소멸자가
    // 불려버리고 멤버로 가지고 있던 light나 camera같은 컴포넌트의 shared_ptr 또한 해제된 상태가 되어버리게 된다.
    // 결국 완전히 잘못된 메모리 접근이 일어나게 된다.
    // 지연된 삭제를 통해 이것을 예방할 수 있다.
    // 그러나 근본적으로 약간 설계가 불안하다고 느껴지는 부분은 스크립트가 실행 도중에 자기 자신을 delete하는 느낌이라는 것이다.
    // 엔진 하부시스템에게 자기 자신의 삭제를 요청하는 식의 구조에서는 어쩔 수 없는 것일까...

    auto destroyedBegin = std::remove_if(
        m_activeComponents.begin(),
        m_activeComponents.end(),
        [](IComponent* pComponent)
        {
            IScript* pScript = static_cast<IScript*>(pComponent);

            const bool isDestroyed = false;
            // const bool isDestroyed = pScript->IsDestroyed();
            if (isDestroyed == false && pScript->IsEnabled() == true)
                pScript->FixedUpdate();

            return isDestroyed;
        }
    );

    m_activeComponents.erase(destroyedBegin, m_activeComponents.cend());
}

void ScriptManager::UpdateScripts()
{
    auto destroyedBegin = std::remove_if(
        m_activeComponents.begin(),
        m_activeComponents.end(),
        [](IComponent* pComponent)
        {
            IScript* pScript = static_cast<IScript*>(pComponent);

            const bool isDestroyed = false;
            // const bool isDestroyed = pScript->IsDestroyed();
            if (isDestroyed == false && pScript->IsEnabled() == true)
                pScript->Update();

            return isDestroyed;
        }
    );

    m_activeComponents.erase(destroyedBegin, m_activeComponents.cend());
}

void ScriptManager::LateUpdateScripts()
{
    auto destroyedBegin = std::remove_if(
        m_activeComponents.begin(),
        m_activeComponents.end(),
        [](IComponent* pComponent)
        {
            IScript* pScript = static_cast<IScript*>(pComponent);

            const bool isDestroyed = false;
            // const bool isDestroyed = pScript->IsDestroyed();
            if (isDestroyed == false && pScript->IsEnabled() == true)
                pScript->LateUpdate();

            return isDestroyed;
        }
    );

    m_activeComponents.erase(destroyedBegin, m_activeComponents.cend());
}
