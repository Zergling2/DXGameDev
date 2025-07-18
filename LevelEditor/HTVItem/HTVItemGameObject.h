#pragma once

#include "HTVItemInterface.h"
#include <ZergEngine\CoreSystem\GamePlayBase\Handle.h>

class HTVItemGameObject : public IHTVItem
{
public:
	HTVItemGameObject(ze::GameObjectHandle hGameObject)
		: m_hGameObject(hGameObject)
	{
	}
	virtual ~HTVItemGameObject()
	{
	}

	virtual void OnSelect();

	ze::GameObjectHandle GetGameObjectHandle() const { return m_hGameObject; }
private:
	ze::GameObjectHandle m_hGameObject;
};
