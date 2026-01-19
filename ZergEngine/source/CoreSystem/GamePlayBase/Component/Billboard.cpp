#include <ZergEngine\CoreSystem\GamePlayBase\Component\Billboard.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\BillboardManager.h>
#include <ZergEngine\CoreSystem\GamePlayBase\GameObject.h>

using namespace ze;

Billboard::Billboard(GameObject& owner) noexcept
	: IComponent(owner, BillboardManager::GetInstance()->AssignUniqueId())
	, m_billboardType(BillboardType::CylindricalY)
	, m_uvScale(1.0f, 1.0f)
	, m_uvOffset(0.0f, 0.0f)
	, m_quadSize(2.0f, 2.0f)
	, m_spMaterial()
{
	XMStoreFloat4x4A(&m_worldMatrixCache, XMMatrixIdentity());
}

IComponentManager* Billboard::GetComponentManager() const
{
	return BillboardManager::GetInstance();
}
