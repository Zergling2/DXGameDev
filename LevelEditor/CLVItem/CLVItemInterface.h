#pragma once

#include "..\framework.h"

class ICLVItem
{
public:
	ICLVItem() = default;
	virtual ~ICLVItem() = default;

	virtual void OnSelect() {}
};
