#include <ZergEngine\CoreSystem\DataStructure\Ray.h>

using namespace ze;

void XM_CALLCONV Ray::Transform(FXMMATRIX matrix) noexcept
{
	XMStoreFloat3A(&m_origin, XMVector3TransformCoord(XMLoadFloat3A(&m_origin), matrix));
	XMStoreFloat3A(&m_direction, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3A(&m_direction), matrix)));
}
