#pragma once

#include <ZergEngine\ZergEngine.h>

class ListenServerClientStarter : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	ListenServerClientStarter(ze::GameObject& owner);
	virtual ~ListenServerClientStarter() = default;

	virtual void Start() override;
};
