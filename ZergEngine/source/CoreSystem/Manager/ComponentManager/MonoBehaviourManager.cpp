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

    pMonoBehaviour->OnFlag(ComponentFlag::Enabled);

    if (pMonoBehaviour->m_pGameObject->IsPending())
        return;

    pMonoBehaviour->OnEnable();

    // Start() �Լ��� ȣ��� ���� ���� Start() �Լ� ȣ�� ��⿭�� ��������� ���� ���
    if (pMonoBehaviour->IsStartCalled() == false && pMonoBehaviour->IsOnTheStartingQueue() == false)
        this->AddToStartQueue(pMonoBehaviour);
}

void MonoBehaviourManager::RequestDisable(MonoBehaviour* pMonoBehaviour)
{
    if (!pMonoBehaviour->IsEnabled())
        return;

    pMonoBehaviour->OffFlag(ComponentFlag::Enabled);

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

    // Start() ȣ�� ť�� ����ִٰ� ���� �����ӿ� ���ŵǴ� ��� StartingQueue���� ������ �����ϱ� ���� �ε����� ����صд�.
    pMonoBehaviour->m_startingQueueIndex = static_cast<uint32_t>(m_startQueue.size() - 1);
    pMonoBehaviour->OnFlag(ComponentFlag::OnStartingQueue);
}

void MonoBehaviourManager::AwakeDeployedComponents()
{
    for (MonoBehaviour* pMonoBehaviour : m_awakeQueue)
    {
        pMonoBehaviour->Awake();	// Awake�� Ȱ��ȭ ���ο� ���� ���� ȣ��
        if (pMonoBehaviour->IsEnabled())
        {
            pMonoBehaviour->OnEnable();
            this->AddToStartQueue(pMonoBehaviour);	// Ȱ��ȭ�� ä�� ������ ���۵Ǵ� ��� Start ��⿭�� �߰�
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
        pScript->OnFlag(ComponentFlag::StartCalled);
        pScript->OffFlag(ComponentFlag::OnStartingQueue);
        pScript->m_startingQueueIndex = (std::numeric_limits<uint32_t>::max)();
    }

    m_startQueue.clear();
}

void MonoBehaviourManager::FixedUpdate()
{
    // Deferred Remove ������� ����.
    // ��� ���� ����� �ܺ� ���ͷ����� �ջ��� ���� ���� �ε����� �����Ѵٴ��� �ؾ��ϰ�
    // �׷��� �ϴ��� ���� ����ó���� ���� �ȴ�.
    
    // ���� ����� �ִ�. ��ũ��Ʈ �Ŵ����� bool isUpdating�� ���� ������Ʈ���� ��ũ��Ʈ�� �ε����� ����ΰ�
    // ���� ��ȸ �� ��� ������ �̷������ �ڿ��� �̵��� ������Ʈ�� ������Ʈ�� �����Ǵ� ���� ���� ���ִ� ������ Update�� ���ְ� ��������
    // �̵���Ų�ٴ���... �׷��� �̷� ������ �ϸ� ���⵵�� �ö󰡰� �ȴ�.

    for (size_t i = 0; i < m_directAccessGroup.size(); ++i)
    {
        MonoBehaviour* pScript = static_cast<MonoBehaviour*>(m_directAccessGroup[i]);
        if (pScript->IsEnabled())
            pScript->FixedUpdate();
    }
}

void MonoBehaviourManager::Update()
{
    for (size_t i = 0; i < m_directAccessGroup.size(); ++i)
    {
        MonoBehaviour* pScript = static_cast<MonoBehaviour*>(m_directAccessGroup[i]);
        if (pScript->IsEnabled())
            pScript->Update();
    }
}

void MonoBehaviourManager::LateUpdate()
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
    // OnDestroy()���� �ٸ� ��ũ��Ʈ�� Destroy �ϴ� ��츦 ����Ͽ�
    // m_destroyed.size()�� �Ź� Ȯ���Ͽ� ���� ������ ���� �� �߰��� �ı� ���� ��ũ��Ʈ�鵵 ��� OnDestroy�� ȣ��� �� �ְ� �Ѵ�.
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
            assert(m_startQueue.size() > 0);
            const uint32_t lastIndex = static_cast<uint32_t>(m_startQueue.size() - 1);
            const uint32_t startingQueueIndex = pScript->m_startingQueueIndex;

            if (startingQueueIndex != lastIndex)
            {
                std::swap(m_startQueue[lastIndex], m_startQueue[startingQueueIndex]);
                m_startQueue[startingQueueIndex]->m_startingQueueIndex = startingQueueIndex;   // �ùٸ��� ������Ʈ
            }
            m_startQueue.pop_back();
        }
    }

    // m_destroyed ���ʹ� IComponentManager::RemoveDestroyedComponents() �Լ����� ���ǹǷ� ���⼭ clear �ϸ� �ȵ�.

    IComponentManager::RemoveDestroyedComponents();
}
