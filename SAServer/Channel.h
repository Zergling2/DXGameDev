#pragma once

#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <memory>
#include <winppy/Network/Packet.h>
#include "Contents.h"
#include "GameRoom.h"

class Player;

namespace winppy
{
	class TCPServer;
}

class Channel
{
public:
	Channel(uint8_t id, size_t maxPlayer);
	Channel(const Channel&) = delete;
	Channel& operator=(const Channel&) = delete;
	Channel(Channel&&) noexcept = default;
	Channel& operator=(Channel&&) noexcept = default;

	size_t GetMaxPlayer() const { return m_maxPlayer; }
	size_t GetNumOfPlayers() const { return m_players.size(); }
	bool AddPlayer(Player* pPlayer);
	void RemovePlayer(Player* pPlayer);
	void BroadcastPacket(winppy::TCPServer& server, winppy::Packet packet) const;
	void BroadcastPacketToPlayersNotInRoom(winppy::TCPServer& server, winppy::Packet packet) const;
	GameRoom* CreateGameRoom(GameRoomTeamFormat tf, const wchar_t* roomName);
	void RemoveGameRoom(uint64_t roomId);
	GameRoom* FindRoom(uint64_t roomId) const;
	std::vector<winppy::Packet> CreateGameRoomListPackets(uint32_t reqContextNo) const;
private:
	uint64_t CreateGameRoomId() { return m_nextGameRoomId++; }
private:
	uint8_t m_id;	// index
	size_t m_maxPlayer;
	std::unordered_set<Player*> m_players;
	uint64_t m_nextGameRoomId;
	std::stack<uint16_t> m_gameRoomNoStk;
	std::unordered_map<uint64_t, std::unique_ptr<GameRoom>> m_gameRooms;
};
