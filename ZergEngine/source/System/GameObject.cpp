#include <ZergEngine\System\GameObject.h>
#include <ZergEngine\System\GameObjectManager.h>
#include <ZergEngine\System\Component\Transform.h>
#include <ZergEngine\System\Scene.h>

using namespace zergengine;

GameObject::GameObject(bool isDeferred, bool isTerrain, const wchar_t* name)
	: m_dontDestroyOnLoad(false)
	, m_isDeferred(isDeferred)
	, m_destroyed(false)
	, m_isTerrain(isTerrain)
	, m_transform()
	, m_components()
{
	StringCbCopyW(m_name, sizeof(m_name), name);
}

void GameObject::DestroyAllComponents()
{
	for (auto& component : m_components)
		ComponentSystem::RemoveComponentFromSystemOnly(component);	// ������Ʈ ����Ʈ�� .clear() �ϹǷ� SkipSearch ���� �Լ� �̿�

	m_components.clear();
}
