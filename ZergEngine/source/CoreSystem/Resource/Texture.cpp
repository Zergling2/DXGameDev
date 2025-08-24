#include <ZergEngine\CoreSystem\Resource\Texture.h>

using namespace ze;

SIZE Texture2D::GetResolution() const
{
	SIZE resolution{ 0, 0 };

	do
	{
		ID3D11Texture2D* pTex2D = this->GetTex2DComInterface();

		if (!pTex2D)
			break;

		D3D11_TEXTURE2D_DESC texDesc;
		pTex2D->GetDesc(&texDesc);

		resolution.cx = texDesc.Width;
		resolution.cy = texDesc.Height;
	} while (false);

	return resolution;
}
