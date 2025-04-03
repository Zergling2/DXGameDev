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
    // �ϴ��� Deferred Remove�� �̿�.
    // ��� ���� ����� �ܺ� ���ͷ����� �ջ��� ���� ���� �ε����� �����Ѵٴ��� �ؾ��ϰ�
    // �׷��� �ϴ��� ���� ����ó���� ���� �ȴ�.
    
    // ���� ����� �ִ�. ��ũ��Ʈ �Ŵ����� bool isUpdating�� ���� ������Ʈ���� ��ũ��Ʈ�� �ε����� ����ΰ�
    // ���� ��ȸ �� ��� ������ �̷������ �ڿ��� ������ ������Ʈ�� ������Ʈ�� �����Ǵ� ���� ���� ���ִ� ������ Update�� ���ְ� ��������
    // �̵���Ų�ٴ���... �׷��� �̷� ������ �ϸ� ���⵵�� �ö󰡰� �ȴ�.

    // �׸��� Deferred remove�� �ʿ��� ���� �߿��� ����
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
