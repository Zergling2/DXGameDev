#pragma once

#include "HTVItemInterface.h"

// ÄÄÆ÷³ÍÆ® ¸®½ºÆ® ºä ¹× ÀÎ½ºÆåÅÍ ºä Á¤¸® °´Ã¼
class HTVItemEmpty : public IHTVItem
{
public:
	HTVItemEmpty() = default;
	virtual ~HTVItemEmpty() = default;

	virtual void OnSelect() override;
};
