#include <ZergEngine\CoreSystem\GamePlayBase\Component\Transform.h>
#include <ZergEngine\CoreSystem\Manager\ComponentManager\TransformManager.h>

using namespace ze;

Transform::Transform() noexcept
	: IComponent(TransformManager.AssignUniqueId())
{
	XMStoreFloat3A(&m_scale, XMVectorSplatOne());
	XMStoreFloat4A(&m_rotation, XMQuaternionIdentity());
	XMStoreFloat3A(&m_position, XMVectorZero());
}

IComponentManager* Transform::GetComponentManager() const
{
	return &TransformManager;
}
