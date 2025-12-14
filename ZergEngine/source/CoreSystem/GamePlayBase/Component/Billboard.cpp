#include <ZergEngine\CoreSystem\GamePlayBase\Component\Billboard.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\BillboardManager.h>

using namespace ze;

Billboard::Billboard() noexcept
	: IComponent(BillboardManager::GetInstance()->AssignUniqueId())
	, m_billboardType(BillboardType::Spherical)
	, m_billboardProj(BillboardProjection::Perspective)
	, m_spMaterial()
{
}

IComponentManager* Billboard::GetComponentManager() const
{
	return BillboardManager::GetInstance();
}
