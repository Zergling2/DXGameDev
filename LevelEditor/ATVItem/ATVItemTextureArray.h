#pragma once

#include "ATVItemInterface.h"

class ATVItemTextureArray : public IATVItem
{
public:
	ATVItemTextureArray(ze::Texture2D texture)
		: IATVItem(ATV_ITEM_TYPE::TEXTURE_ARRAY)
		, m_texture(std::move(texture))
	{
	}
	virtual ~ATVItemTextureArray() = default;
private:
	ze::Texture2D m_texture;
};
