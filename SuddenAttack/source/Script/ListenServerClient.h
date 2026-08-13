#pragma once

#include <ZergEngine\ZergEngine.h>
#include <enet\enet.h>

class Network;

class ListenServerClient : public ze::MonoBehaviour
{
	DECLARE_MONOBEHAVIOUR_TYPE
public:
	ListenServerClient(ze::GameObject& owner);
	virtual ~ListenServerClient() = default;

	virtual void Update() override;
	virtual void OnDestroy() override;

	void StartClient(uint32_t serverIP, uint16_t serverPort);
	void CloseClient();

	void SetNetworkScriptHandle(ze::ComponentHandle<Network> hScript) { m_hScriptNetwork = hScript; }
private:
	ENetHost* m_pClient;
	ENetPeer* m_pPeer;		// host
	ze::ComponentHandle<Network> m_hScriptNetwork;
};
