#pragma once

#include "ATVItemInterface.h"

class ATVItemTexture : public IATVItem
{
public:
	ATVItemTexture(ze::Texture2D texture)
		: IATVItem(ATV_ITEM_TYPE::TEXTURE)
		, m_texture(std::move(texture))
	{
	}
	virtual ~ATVItemTexture() = default;

	virtual void OnSelect() override;
private:
	ze::Texture2D m_texture;
};
