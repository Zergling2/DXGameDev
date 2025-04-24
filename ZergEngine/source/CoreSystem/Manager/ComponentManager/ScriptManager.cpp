#include <ZergEngine\CoreSystem\Manager\ComponentManager\ScriptManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\Component\ScriptInterface.h>

namespace ze
{
    ScriptManagerImpl ScriptManager;
}

using namespace ze;

ScriptManagerImpl::ScriptManagerImpl()
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

void ScriptManagerImpl::FixedUpdateScripts()
{
    // Deferred Remove ������� ����.
    // ��� ���� ����� �ܺ� ���ͷ����� �ջ��� ���� ���� �ε����� �����Ѵٴ��� �ؾ��ϰ�
    // �׷��� �ϴ��� ���� ����ó���� ���� �ȴ�.
    
    // ���� ����� �ִ�. ��ũ��Ʈ �Ŵ����� bool isUpdating�� ���� ������Ʈ���� ��ũ��Ʈ�� �ε����� ����ΰ�
    // ���� ��ȸ �� ��� ������ �̷������ �ڿ��� ������ ������Ʈ�� ������Ʈ�� �����Ǵ� ���� ���� ���ִ� ������ Update�� ���ְ� ��������
    // �̵���Ų�ٴ���... �׷��� �̷� ������ �ϸ� ���⵵�� �ö󰡰� �ȴ�.

    // �׸��� ���� ������ �ʿ��� ���� �߿��� ����
    // ��ũ��Ʈ���� Destroy(gameObject) �Ǵ� Destroy(this) ȣ�� �� return���� �ʰ� ��� �����ϴ� ���
    // Destroy �Լ����� ���� ������ ��û�س��� �ʰ� �ٷ� �޸� ������ �ع����� �߸��� �޸� ������ �ϰ� �ȴ�.

    // �̷� ��쵵 ���� �� �ִ�.
    // ������Ʈ�� Light�� Camera �̷��� ����� shared_ptr �ΰ� �����ϰ� ���� �� �ִµ�
    // ������Ʈ�� ���(light, camera���� ������Ʈ)�� Destroy�ϰ� �ڱ� �ڽ��� Destroy �Ѵ����� �� �ؿ��� ��� light�� camera�� �����ϴ� �ڵ带
    // ����Ѵٰ� �����ϸ� ���� ��ũ��Ʈ�� deferred destroy ������� ���ŵ��� �ʰ� Destroy���� �ٷ� ���ŵǾ��ٸ� ��ũ��Ʈ Ŭ������ �Ҹ��ڰ�
    // �ҷ������� ����� ������ �ִ� light�� camera���� ������Ʈ�� shared_ptr ���� ������ ���°� �Ǿ������ �ȴ�.
    // �ᱹ ������ �߸��� �޸� ������ �Ͼ�� �ȴ�.
    // ������ ������ ���� �̰��� ������ �� �ִ�.
    // �׷��� �ٺ������� �ణ ���谡 �Ҿ��ϴٰ� �������� �κ��� ��ũ��Ʈ�� ���� ���߿� �ڱ� �ڽ��� delete�ϴ� �����̶�� ���̴�.
    // ���� �Ϻνý��ۿ��� �ڱ� �ڽ��� ������ ��û�ϴ� ���� ���������� ��¿ �� ���� ���ϱ�...

    for (size_t i = 0; i < m_activeComponents.size(); ++i)
    {
        IScript* pScript = static_cast<IScript*>(m_activeComponents[i]);

        if (pScript->IsEnabled())
            pScript->FixedUpdate();
    }
}

void ScriptManagerImpl::UpdateScripts()
{
    for (size_t i = 0; i < m_activeComponents.size(); ++i)
    {
        IScript* pScript = static_cast<IScript*>(m_activeComponents[i]);

        if (pScript->IsEnabled())
            pScript->Update();
    }
}

void ScriptManagerImpl::LateUpdateScripts()
{
    for (size_t i = 0; i < m_activeComponents.size(); ++i)
    {
        IScript* pScript = static_cast<IScript*>(m_activeComponents[i]);

        if (pScript->IsEnabled())
            pScript->LateUpdate();
    }
}

void ScriptManagerImpl::RemoveDestroyedComponents()
{
    // OnDestroy()���� �ٸ� ��ũ��Ʈ�� Destroy �ϴ� ��츦 ����Ͽ�
    // m_destroyed.size()�� �Ź� Ȯ���Ͽ� �߰��� �ı� ���� ��ũ��Ʈ�鵵 ��� OnDestroy�� ȣ��� �� �ְ� �Ѵ�.
    for (size_t i = 0; i < m_destroyed.size(); ++i)
        static_cast<IScript*>(m_destroyed[i])->OnDestroy();

    IComponentManager::RemoveDestroyedComponents();
}
