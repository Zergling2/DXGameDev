#pragma once

#include <ZergEngine\ZergEngine.h>

class ListenServerStarter : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	ListenServerStarter(ze::GameObject& owner);
	virtual ~ListenServerStarter() = default;

	virtual void Start() override;
};
