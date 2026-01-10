#include <ZergEngine\CoreSystem\Resource\Texture.h>

using namespace ze;

SIZE Texture2D::GetResolution() const
{
	ID3D11Texture2D* pTex2D = this->GetTexture2D();

	if (!pTex2D)
		return SIZE{ 0, 0 };

	D3D11_TEXTURE2D_DESC texDesc;
	pTex2D->GetDesc(&texDesc);

	return SIZE{ static_cast<LONG>(texDesc.Width), static_cast<LONG>(texDesc.Height) };
}
