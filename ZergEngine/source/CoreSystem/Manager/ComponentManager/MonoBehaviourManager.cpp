#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MonoBehaviour.h>

namespace ze
{
    MonoBehaviourManagerImpl MonoBehaviourManager;
}

using namespace ze;

MonoBehaviourManagerImpl::MonoBehaviourManagerImpl()
    : m_startingScripts()
{
}

MonoBehaviourManagerImpl::~MonoBehaviourManagerImpl()
{
}

void MonoBehaviourManagerImpl::Init(void* pDesc)
{
}

void MonoBehaviourManagerImpl::Release()
{
}

void MonoBehaviourManagerImpl::AddToStartingQueue(MonoBehaviour* pScript)
{
    assert(!pScript->IsOnTheStartingQueue());
    assert(!pScript->IsStartCalled());

    m_startingScripts.push_back(pScript);

    // Start() 호출 큐에 들어있다가 직전 프레임에 제거되는 경우 StartingQueue에서 빠르게 제거하기 위해 인덱스를 기록해둔다.
    pScript->m_startingQueueIndex = static_cast<uint32_t>(m_startingScripts.size() - 1);
    pScript->OnFlag(COMPONENT_FLAG::ON_STARTING_QUEUE);
}

void MonoBehaviourManagerImpl::CallStart()
{
    for (MonoBehaviour* pScript : m_startingScripts)
    {
        assert(pScript->IsOnTheStartingQueue());
        assert(!pScript->IsStartCalled());

        pScript->Start();
        pScript->OnFlag(COMPONENT_FLAG::START_CALLED);
        pScript->OffFlag(COMPONENT_FLAG::ON_STARTING_QUEUE);
        pScript->m_startingQueueIndex = std::numeric_limits<uint32_t>::max();
    }

    m_startingScripts.clear();
}

void MonoBehaviourManagerImpl::FixedUpdateScripts()
{
    // Deferred Remove 방식으로 구현.
    // 즉시 삭제 방법은 외부 이터레이터 손상을 막기 위해 인덱스로 접근한다던가 해야하고
    // 그렇게 하더라도 여러 예외처리가 들어가게 된다.
    
    // 물론 방법은 있다. 스크립트 매니저에 bool isUpdating과 현재 업데이트중인 스크립트의 인덱스를 적어두고
    // 만약 순회 중 즉시 삭제가 이루어지면 뒤에서 땡겨진 컴포넌트가 업데이트가 생략되는 일이 없게 해주는 식으로 Update를 해주고 앞쪽으로
    // 이동시킨다던지... 그러나 이런 식으로 하면 복잡도가 올라가게 된다.

    for (size_t i = 0; i < m_activeComponents.size(); ++i)
    {
        MonoBehaviour* pScript = static_cast<MonoBehaviour*>(m_activeComponents[i]);
        if (pScript->IsEnabled())
            pScript->FixedUpdate();
    }
}

void MonoBehaviourManagerImpl::UpdateScripts()
{
    for (size_t i = 0; i < m_activeComponents.size(); ++i)
    {
        MonoBehaviour* pScript = static_cast<MonoBehaviour*>(m_activeComponents[i]);
        if (pScript->IsEnabled())
            pScript->Update();
    }
}

void MonoBehaviourManagerImpl::LateUpdateScripts()
{
    for (size_t i = 0; i < m_activeComponents.size(); ++i)
    {
        MonoBehaviour* pScript = static_cast<MonoBehaviour*>(m_activeComponents[i]);
        if (pScript->IsEnabled())
            pScript->LateUpdate();
    }
}

void MonoBehaviourManagerImpl::RemoveDestroyedComponents()
{
    // OnDestroy()에서 다른 스크립트를 Destroy 하는 경우를 고려하여
    // m_destroyed.size()를 매번 확인하여 추가된 파괴 예정 스크립트들도 모두 OnDestroy가 호출될 수 있게 한다.
    for (size_t i = 0; i < m_destroyed.size(); ++i)
    {
        MonoBehaviour* pScript = static_cast<MonoBehaviour*>(m_destroyed[i]);
        if (pScript->IsEnabled())   // 활성화 되어있는 스크립트에 한해서만 OnDestroy() 호출
        {
            pScript->Disable();
            pScript->OnDestroy();
        }

        // 파괴되는 스크립트가 Start 대기열에 있는 경우 Start 대기열에 댕글링 포인터가 남지 않게 제거
        if (pScript->IsOnTheStartingQueue())
        {
            assert(m_startingScripts.size() > 0);
            const uint32_t lastIndex = static_cast<uint32_t>(m_startingScripts.size() - 1);
            const uint32_t startingQueueIndex = pScript->m_startingQueueIndex;

            if (startingQueueIndex != lastIndex)
            {
                std::swap(m_startingScripts[lastIndex], m_startingScripts[startingQueueIndex]);
                m_startingScripts[startingQueueIndex]->m_startingQueueIndex = startingQueueIndex;   // 올바르게 업데이트
            }
            m_startingScripts.pop_back();
        }
    }

    IComponentManager::RemoveDestroyedComponents();
}
