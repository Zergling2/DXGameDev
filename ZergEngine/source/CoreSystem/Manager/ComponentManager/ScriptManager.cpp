#include <ZergEngine\CoreSystem\Manager\ComponentManager\ScriptManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ScriptInterface.h>

namespace ze
{
    ScriptManagerImpl ScriptManager;
}

using namespace ze;

ScriptManagerImpl::ScriptManagerImpl()
    : m_startingScripts()
{
}

ScriptManagerImpl::~ScriptManagerImpl()
{
}

void ScriptManagerImpl::Init(void* pDesc)
{
}

void ScriptManagerImpl::Release()
{
}

void ScriptManagerImpl::AddToStartingQueue(IScript* pScript)
{
    assert((pScript->GetFlag() & CF_START_CALLED) == FALSE);
    assert((pScript->GetFlag() & CF_ON_STARTING_QUEUE) == FALSE);

    m_startingScripts.push_back(pScript);

    // Start() ȣ�� ť�� ����ִٰ� ���� �����ӿ� ���ŵǴ� ��� StartingQueue���� ������ �����ϱ� ���� �ε����� ����صд�.
    pScript->m_startingQueueIndex = static_cast<uint32_t>(m_startingScripts.size() - 1);
    pScript->OnFlag(CF_ON_STARTING_QUEUE);
}

void ScriptManagerImpl::CallStart()
{
    for (IScript* pScript : m_startingScripts)
    {
        assert((pScript->GetFlag() & CF_ON_STARTING_QUEUE) != FALSE);
        assert((pScript->GetFlag() & CF_START_CALLED) == FALSE);

        pScript->Start();
        pScript->OnFlag(CF_START_CALLED);
        pScript->OffFlag(CF_ON_STARTING_QUEUE);
        pScript->m_startingQueueIndex = std::numeric_limits<uint32_t>::max();
    }

    m_startingScripts.clear();
}

void ScriptManagerImpl::FixedUpdateScripts()
{
    // Deferred Remove ������� ����.
    // ��� ���� ����� �ܺ� ���ͷ����� �ջ��� ���� ���� �ε����� �����Ѵٴ��� �ؾ��ϰ�
    // �׷��� �ϴ��� ���� ����ó���� ���� �ȴ�.
    
    // ���� ����� �ִ�. ��ũ��Ʈ �Ŵ����� bool isUpdating�� ���� ������Ʈ���� ��ũ��Ʈ�� �ε����� ����ΰ�
    // ���� ��ȸ �� ��� ������ �̷������ �ڿ��� ������ ������Ʈ�� ������Ʈ�� �����Ǵ� ���� ���� ���ִ� ������ Update�� ���ְ� ��������
    // �̵���Ų�ٴ���... �׷��� �̷� ������ �ϸ� ���⵵�� �ö󰡰� �ȴ�.

    for (size_t i = 0; i < m_enabledComponents.size(); ++i)
        static_cast<IScript*>(m_enabledComponents[i])->FixedUpdate();
}

void ScriptManagerImpl::UpdateScripts()
{
    for (size_t i = 0; i < m_enabledComponents.size(); ++i)
        static_cast<IScript*>(m_enabledComponents[i])->Update();
}

void ScriptManagerImpl::LateUpdateScripts()
{
    for (size_t i = 0; i < m_enabledComponents.size(); ++i)
        static_cast<IScript*>(m_enabledComponents[i])->LateUpdate();
}

void ScriptManagerImpl::RemoveDestroyedComponents()
{
    // OnDestroy()���� �ٸ� ��ũ��Ʈ�� Destroy �ϴ� ��츦 ����Ͽ�
    // m_destroyed.size()�� �Ź� Ȯ���Ͽ� �߰��� �ı� ���� ��ũ��Ʈ�鵵 ��� OnDestroy�� ȣ��� �� �ְ� �Ѵ�.
    for (size_t i = 0; i < m_destroyed.size(); ++i)
    {
        IScript* pScript = static_cast<IScript*>(m_destroyed[i]);
        if (pScript->IsEnabled())   // Ȱ��ȭ �Ǿ��ִ� ��ũ��Ʈ�� ���ؼ��� OnDestroy() ȣ��
            pScript->OnDestroy();

        // �ı��Ǵ� ��ũ��Ʈ�� Start ��⿭�� �ִ� ��� ��۸� �����Ͱ� ���� �ʰ� ����
        if (pScript->GetFlag() & CF_ON_STARTING_QUEUE)
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
