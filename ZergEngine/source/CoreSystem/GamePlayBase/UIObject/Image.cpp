#include <ZergEngine\CoreSystem\GamePlayBase\UIObject\Image.h>

using namespace ze;

Image::Image(uint64_t id, UIOBJECT_FLAG flag, PCWSTR name)
	: ISizeColorUIObject(id, flag, name, UIOBJECT_TYPE::IMAGE)
	, m_nativeSize(false)
	, m_texture()
{
}

void Image::SetTexture(const Texture2D& texture)
{
	m_texture = texture;

	if (m_nativeSize)
		this->UpdateToNativeSize();
}

void Image::SetNativeSize(bool b)
{
	m_nativeSize = b;

	if (m_nativeSize)
		this->UpdateToNativeSize();
}

void Image::UpdateToNativeSize()
{
	if (m_texture.GetTex2DComInterface())
	{
		D3D11_TEXTURE2D_DESC desc;
		m_texture.GetTexture2DDesc(&desc);

		this->SetSize(XMFLOAT2A(static_cast<FLOAT>(desc.Width), static_cast<FLOAT>(desc.Height)));
	}
}
