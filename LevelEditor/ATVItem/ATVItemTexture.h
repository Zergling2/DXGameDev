#pragma once

#include "ATVItemInterface.h"
#include <ZergEngine\CoreSystem\Resource\Texture.h>

class ATVItemTexture : public IATVItem
{
public:
	ATVItemTexture() = default;
	virtual ~ATVItemTexture() = default;

	virtual ATVItemType GetType() const { return ATVItemType::Texture; }
	virtual void OnSelect() override;
public:
	ze::Texture2D m_texture;
};
