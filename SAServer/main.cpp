#include <WinSock2.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "GameServer.h"

int main(void)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return -1;

	GameServer server;

	winppy::TCPServerConfig config;
	config.m_bindAddr = nullptr;
	config.m_bindPort = SASERVER_PORT;
	config.m_numOfWorkerThreads = 6;
	config.m_numOfConcurrentThreads = 4;
	config.m_tcpNoDelay = true;
	config.m_headerCode = HEADER_CODE;
	config.m_maxSessionCount = 5000;
	int ret = server.Run(config);
	if (ret == 0)
		server.Shutdown();

	WSACleanup();

	return 0;
}
