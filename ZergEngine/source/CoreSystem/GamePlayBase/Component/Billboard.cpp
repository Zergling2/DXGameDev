#include <ZergEngine\CoreSystem\GamePlayBase\Component\Billboard.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\BillboardManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

Billboard::Billboard() noexcept
	: IComponent(BillboardManager::GetInstance()->AssignUniqueId())
	, m_billboardType(BillboardType::CylindricalY)
	, m_quadWidth(2.0f)
	, m_quadHeight(2.0f)
	, m_spMaterial()
{
	XMStoreFloat4x4A(&m_worldMatrixCache, XMMatrixIdentity());
}

IComponentManager* Billboard::GetComponentManager() const
{
	return BillboardManager::GetInstance();
}
