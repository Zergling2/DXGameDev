#pragma once

#include "HTVItemInterface.h"

class HTVItemRoot : public IHTVItem
{
public:
	HTVItemRoot() = default;
	virtual ~HTVItemRoot() = default;

	virtual void OnSelect();
};
