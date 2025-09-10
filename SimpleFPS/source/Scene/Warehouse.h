#pragma once

#include <ZergEngine\ZergEngine.h>

class Warehouse : public ze::IScene
{
	ZE_DECLARE_SCENE(Warehouse);
public:
	virtual void OnLoadScene() override;
};
