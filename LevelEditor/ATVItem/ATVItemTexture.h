#pragma once

#include "ATVItemInterface.h"

class ATVItemTexture : public IATVItem
{
public:
	ATVItemTexture()
		: IATVItem(ATV_ITEM_TYPE::TEXTURE)
	{
	}
	virtual ~ATVItemTexture() = default;

	virtual void OnSelect() override;
public:
	ze::Texture2D m_texture;
};
