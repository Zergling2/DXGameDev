#pragma once

#include "HTVItemInterface.h"

// ������Ʈ ����Ʈ �� �� ������Ʈ �ν����� �� ���� ��ü
class HTVItemEmpty : public IHTVItem
{
public:
	HTVItemEmpty() = default;
	virtual ~HTVItemEmpty() = default;

	virtual void OnSelect();
};
