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

    // Start() ȣ�� ť�� ����ִٰ� ���� �����ӿ� ���ŵǴ� ��� StartingQueue���� ������ �����ϱ� ���� �ε����� ����صд�.
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
    // Deferred Remove ������� ����.
    // ��� ���� ����� �ܺ� ���ͷ����� �ջ��� ���� ���� �ε����� �����Ѵٴ��� �ؾ��ϰ�
    // �׷��� �ϴ��� ���� ����ó���� ���� �ȴ�.
    
    // ���� ����� �ִ�. ��ũ��Ʈ �Ŵ����� bool isUpdating�� ���� ������Ʈ���� ��ũ��Ʈ�� �ε����� ����ΰ�
    // ���� ��ȸ �� ��� ������ �̷������ �ڿ��� ������ ������Ʈ�� ������Ʈ�� �����Ǵ� ���� ���� ���ִ� ������ Update�� ���ְ� ��������
    // �̵���Ų�ٴ���... �׷��� �̷� ������ �ϸ� ���⵵�� �ö󰡰� �ȴ�.

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
    // OnDestroy()���� �ٸ� ��ũ��Ʈ�� Destroy �ϴ� ��츦 ����Ͽ�
    // m_destroyed.size()�� �Ź� Ȯ���Ͽ� �߰��� �ı� ���� ��ũ��Ʈ�鵵 ��� OnDestroy�� ȣ��� �� �ְ� �Ѵ�.
    for (size_t i = 0; i < m_destroyed.size(); ++i)
    {
        MonoBehaviour* pScript = static_cast<MonoBehaviour*>(m_destroyed[i]);
        if (pScript->IsEnabled())   // Ȱ��ȭ �Ǿ��ִ� ��ũ��Ʈ�� ���ؼ��� OnDestroy() ȣ��
        {
            pScript->Disable();
            pScript->OnDestroy();
        }

        // �ı��Ǵ� ��ũ��Ʈ�� Start ��⿭�� �ִ� ��� Start ��⿭�� ��۸� �����Ͱ� ���� �ʰ� ����
        if (pScript->IsOnTheStartingQueue())
        {
            assert(m_startingScripts.size() > 0);
            const uint32_t lastIndex = static_cast<uint32_t>(m_startingScripts.size() - 1);
            const uint32_t startingQueueIndex = pScript->m_startingQueueIndex;

            if (startingQueueIndex != lastIndex)
            {
                std::swap(m_startingScripts[lastIndex], m_startingScripts[startingQueueIndex]);
                m_startingScripts[startingQueueIndex]->m_startingQueueIndex = startingQueueIndex;   // �ùٸ��� ������Ʈ
            }
            m_startingScripts.pop_back();
        }
    }

    IComponentManager::RemoveDestroyedComponents();
}
