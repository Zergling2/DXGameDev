#include <ZergEngine\CoreSystem\Math.h>

using namespace ze;

void Math::ExtractFrustumPlanesInWorldSpace(XMMATRIX vp, XMFLOAT4A planes[6])
{
	XMFLOAT4X4A vp4x4;
	XMStoreFloat4x4A(&vp4x4, vp);

	// Left
	planes[0].x = vp4x4(0, 0) + vp4x4(0, 3);
	planes[0].y = vp4x4(1, 0) + vp4x4(1, 3);
	planes[0].z = vp4x4(2, 0) + vp4x4(2, 3);
	planes[0].w = vp4x4(3, 0) + vp4x4(3, 3);

	// Right
	planes[1].x = vp4x4(0, 3) - vp4x4(0, 0);
	planes[1].y = vp4x4(1, 3) - vp4x4(1, 0);
	planes[1].z = vp4x4(2, 3) - vp4x4(2, 0);
	planes[1].w = vp4x4(3, 3) - vp4x4(3, 0);

	// Bottom
	planes[2].x = vp4x4(0, 1) + vp4x4(0, 3);
	planes[2].y = vp4x4(1, 1) + vp4x4(1, 3);
	planes[2].z = vp4x4(2, 1) + vp4x4(2, 3);
	planes[2].w = vp4x4(3, 1) + vp4x4(3, 3);

	// Top
	planes[3].x = vp4x4(0, 3) - vp4x4(0, 1);
	planes[3].y = vp4x4(1, 3) - vp4x4(1, 1);
	planes[3].z = vp4x4(2, 3) - vp4x4(2, 1);
	planes[3].w = vp4x4(3, 3) - vp4x4(3, 1);

	// Near
	planes[4].x = vp4x4(0, 2);
	planes[4].y = vp4x4(1, 2);
	planes[4].z = vp4x4(2, 2);
	planes[4].w = vp4x4(3, 2);

	// Far
	planes[5].x = vp4x4(0, 3) - vp4x4(0, 2);
	planes[5].y = vp4x4(1, 3) - vp4x4(1, 2);
	planes[5].z = vp4x4(2, 3) - vp4x4(2, 2);
	planes[5].w = vp4x4(3, 3) - vp4x4(3, 2);

	for (uint32_t i = 0; i < 6; ++i)
	{
		const XMVECTOR v = XMPlaneNormalize(XMLoadFloat4A(&planes[i]));
		XMStoreFloat4A(&planes[i], v);
	}
}
