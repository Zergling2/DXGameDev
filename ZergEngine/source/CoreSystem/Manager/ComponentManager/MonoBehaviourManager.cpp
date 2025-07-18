#include <ZergEngine\CoreSystem\Manager\ComponentManager\MonoBehaviourManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\MonoBehaviour.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

MonoBehaviourManager* MonoBehaviourManager::s_pInstance = nullptr;

MonoBehaviourManager::MonoBehaviourManager()
    : m_awakeQueue(128)
    , m_startQueue(128)
{
    m_awakeQueue.clear();
    m_startQueue.clear();
}

void MonoBehaviourManager::CreateInstance()
{
    assert(s_pInstance == nullptr);

    s_pInstance = new MonoBehaviourManager();
}

void MonoBehaviourManager::DestroyInstance()
{
    assert(s_pInstance != nullptr);

    delete s_pInstance;
    s_pInstance = nullptr;
}

void MonoBehaviourManager::RequestEnable(MonoBehaviour* pMonoBehaviour)
{
    if (pMonoBehaviour->IsEnabled())
        return;

    pMonoBehaviour->OnFlag(COMPONENT_FLAG::ENABLED);

    if (pMonoBehaviour->m_pGameObject->IsPending())
        return;

    pMonoBehaviour->OnEnable();

    // Start() 함수가 호출된 적이 없고 Start() 함수 호출 대기열에 들어있지도 않은 경우
    if (pMonoBehaviour->IsStartCalled() == false && pMonoBehaviour->IsOnTheStartingQueue() == false)
        this->AddToStartQueue(pMonoBehaviour);
}

void MonoBehaviourManager::RequestDisable(MonoBehaviour* pMonoBehaviour)
{
    if (!pMonoBehaviour->IsEnabled())
        return;

    pMonoBehaviour->OffFlag(COMPONENT_FLAG::ENABLED);

    if (pMonoBehaviour->m_pGameObject->IsPending())
        return;

    pMonoBehaviour->OnDisable();
}

void MonoBehaviourManager::Deploy(IComponent* pComponent)
{
    IComponentManager::Deploy(pComponent);

    m_awakeQueue.push_back(static_cast<MonoBehaviour*>(pComponent));
}

void MonoBehaviourManager::AddToStartQueue(MonoBehaviour* pMonoBehaviour)
{
    assert(!pMonoBehaviour->IsOnTheStartingQueue());
    assert(!pMonoBehaviour->IsStartCalled());

    m_startQueue.push_back(pMonoBehaviour);

    // Start() 호출 큐에 들어있다가 직전 프레임에 제거되는 경우 StartingQueue에서 빠르게 제거하기 위해 인덱스를 기록해둔다.
    pMonoBehaviour->m_startingQueueIndex = static_cast<uint32_t>(m_startQueue.size() - 1);
    pMonoBehaviour->OnFlag(COMPONENT_FLAG::ON_STARTING_QUEUE);
}

void MonoBehaviourManager::AwakeDeployedComponents()
{
    for (MonoBehaviour* pMonoBehaviour : m_awakeQueue)
    {
        pMonoBehaviour->Awake();	// Awake는 활성화 여부와 관계 없이 호출
        if (pMonoBehaviour->IsEnabled())
        {
            pMonoBehaviour->OnEnable();
            this->AddToStartQueue(pMonoBehaviour);	// 활성화된 채로 씬에서 시작되는 경우 Start 대기열에 추가
        }
    }

    m_awakeQueue.clear();
}

void MonoBehaviourManager::CallStart()
{
    for (MonoBehaviour* pScript : m_startQueue)
    {
        assert(pScript->IsOnTheStartingQueue());
        assert(!pScript->IsStartCalled());

        pScript->Start();
        pScript->OnFlag(COMPONENT_FLAG::START_CALLED);
        pScript->OffFlag(COMPONENT_FLAG::ON_STARTING_QUEUE);
        pScript->m_startingQueueIndex = std::numeric_limits<uint32_t>::max();
    }

    m_startQueue.clear();
}

void MonoBehaviourManager::FixedUpdateScripts()
{
    // Deferred Remove 방식으로 구현.
    // 즉시 삭제 방법은 외부 이터레이터 손상을 막기 위해 인덱스로 접근한다던가 해야하고
    // 그렇게 하더라도 여러 예외처리가 들어가게 된다.
    
    // 물론 방법은 있다. 스크립트 매니저에 bool isUpdating과 현재 업데이트중인 스크립트의 인덱스를 적어두고
    // 만약 순회 중 즉시 삭제가 이루어지면 뒤에서 이동된 컴포넌트가 업데이트가 생략되는 일이 없게 해주는 식으로 Update를 해주고 앞쪽으로
    // 이동시킨다던지... 그러나 이런 식으로 하면 복잡도가 올라가게 된다.

    for (size_t i = 0; i < m_directAccessGroup.size(); ++i)
    {
        MonoBehaviour* pScript = static_cast<MonoBehaviour*>(m_directAccessGroup[i]);
        if (pScript->IsEnabled())
            pScript->FixedUpdate();
    }
}

void MonoBehaviourManager::UpdateScripts()
{
    for (size_t i = 0; i < m_directAccessGroup.size(); ++i)
    {
        MonoBehaviour* pScript = static_cast<MonoBehaviour*>(m_directAccessGroup[i]);
        if (pScript->IsEnabled())
            pScript->Update();
    }
}

void MonoBehaviourManager::LateUpdateScripts()
{
    for (size_t i = 0; i < m_directAccessGroup.size(); ++i)
    {
        MonoBehaviour* pScript = static_cast<MonoBehaviour*>(m_directAccessGroup[i]);
        if (pScript->IsEnabled())
            pScript->LateUpdate();
    }
}

void MonoBehaviourManager::RemoveDestroyedComponents()
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
            assert(m_startQueue.size() > 0);
            const uint32_t lastIndex = static_cast<uint32_t>(m_startQueue.size() - 1);
            const uint32_t startingQueueIndex = pScript->m_startingQueueIndex;

            if (startingQueueIndex != lastIndex)
            {
                std::swap(m_startQueue[lastIndex], m_startQueue[startingQueueIndex]);
                m_startQueue[startingQueueIndex]->m_startingQueueIndex = startingQueueIndex;   // 올바르게 업데이트
            }
            m_startQueue.pop_back();
        }
    }

    IComponentManager::RemoveDestroyedComponents();
}