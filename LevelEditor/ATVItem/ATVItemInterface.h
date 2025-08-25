#pragma once

#include "..\framework.h"

enum class ATV_ITEM_TYPE : uint8_t
{
	EMPTY,
	FOLDER,
	MATERIAL,
	TEXTURE,
	CUBEMAP
};

class IATVItem abstract
{
public:
	IATVItem(ATV_ITEM_TYPE type)
		: m_type(type)
	{
	}
	virtual ~IATVItem() = default;

	virtual void OnSelect() = 0;

	ATV_ITEM_TYPE GetType() const { return m_type; }
private:
	ATV_ITEM_TYPE m_type;
};
