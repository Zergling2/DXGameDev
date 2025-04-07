#include <ZergEngine\CoreSystem\Math.h>

using namespace ze;

void Math::ExtractFrustumPlanesInWorldSpace(const XMFLOAT4X4A* pViewProjMatrix, XMFLOAT4A planes[6])
{
	// Left
	planes[0].x = pViewProjMatrix->_11 + pViewProjMatrix->_14;
	planes[0].y = pViewProjMatrix->_21 + pViewProjMatrix->_24;
	planes[0].z = pViewProjMatrix->_31 + pViewProjMatrix->_34;
	planes[0].w = pViewProjMatrix->_41 + pViewProjMatrix->_44;

	// Right
	planes[1].x = pViewProjMatrix->_14 - pViewProjMatrix->_11;
	planes[1].y = pViewProjMatrix->_24 - pViewProjMatrix->_21;
	planes[1].z = pViewProjMatrix->_34 - pViewProjMatrix->_31;
	planes[1].w = pViewProjMatrix->_44 - pViewProjMatrix->_41;

	// Bottom
	planes[2].x = pViewProjMatrix->_12 + pViewProjMatrix->_14;
	planes[2].y = pViewProjMatrix->_22 + pViewProjMatrix->_24;
	planes[2].z = pViewProjMatrix->_32 + pViewProjMatrix->_34;
	planes[2].w = pViewProjMatrix->_42 + pViewProjMatrix->_44;
	
	// Top
	planes[3].x = pViewProjMatrix->_14 - pViewProjMatrix->_12;
	planes[3].y = pViewProjMatrix->_24 - pViewProjMatrix->_22;
	planes[3].z = pViewProjMatrix->_34 - pViewProjMatrix->_32;
	planes[3].w = pViewProjMatrix->_44 - pViewProjMatrix->_42;
	
	// Near
	planes[4].x = pViewProjMatrix->_13;
	planes[4].y = pViewProjMatrix->_23;
	planes[4].z = pViewProjMatrix->_33;
	planes[4].w = pViewProjMatrix->_42;

	// Far
	planes[5].x = pViewProjMatrix->_14 - pViewProjMatrix->_13;
	planes[5].y = pViewProjMatrix->_24 - pViewProjMatrix->_23;
	planes[5].z = pViewProjMatrix->_34 - pViewProjMatrix->_33;
	planes[5].w = pViewProjMatrix->_44 - pViewProjMatrix->_43;

	for (uint32_t i = 0; i < 6; ++i)
	{
		const XMVECTOR v = XMPlaneNormalize(XMLoadFloat4A(&planes[i]));
		XMStoreFloat4A(&planes[i], v);
	}
}
