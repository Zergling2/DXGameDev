#pragma once

#include <vector>
#include <winppy/Network/TCPServer.h>
#include <winppy/Network/Packet.h>
#include "Constants.h"
#include "Contents.h"

class Player;

enum class RemovePlayerResult
{
    Normal,
    HostChanged,
    LastPlayerRemoved
};

struct GameRoomPlayer
{
public:
    GameRoomPlayer()
        : m_pPlayer(nullptr)
        , m_state(PlayerState::Unknown)
    {
    }
    GameRoomPlayer(const Player* pPlayer, PlayerState state)
        : m_pPlayer(pPlayer)
        , m_state(state)
    {
    }
    const Player* m_pPlayer;
    PlayerState m_state;
};

class GameRoom
{
public:
    GameRoom(uint64_t id, uint16_t no, GameRoomTeamFormat tf, GameMap map, const wchar_t* name);

    uint64_t GetId() const { return m_id; }
    uint16_t GetNo() const { return m_no; }

    bool ChangeMap(const Player* pRequester, GameMap map);

    GameRoomTeamFormat GetTeamFormat() const { return m_tf; }
    void SetTeamFormat(GameRoomTeamFormat tf) { m_tf = tf; }

    GameMap GetMap() const { return m_map; }
    void SetMap(GameMap map) { m_map = map; }

    GameRoomState GetState() const { return m_state; }
    void SetState(GameRoomState state) { m_state = state; }

    const std::wstring& GetName() const { return m_name; }
    void SetName(const wchar_t* name) { m_name = name; }

    bool AddPlayerAsHost(Player* pPlayer, GameTeam& team);
    bool AddPlayer(Player* pPlayer, GameTeam& team);
    bool MoveTeam(const Player* pPlayer, GameTeam newTeam);
    RemovePlayerResult RemovePlayer(Player* pPlayer);
    const Player* GetHost() const { return m_pHost; }

    void BroadcastPacket(winppy::TCPServer& server, winppy::Packet packet) const;
    void BroadcastPacketExceptPlayer(winppy::TCPServer& server, winppy::Packet packet, const Player* pPlayer) const;
    size_t GetNumOfPlayers() const { return m_redTeam.size() + m_blueTeam.size(); }
    winppy::Packet CreateGameRoomPlayerJoinedPacket(const Player* pPlayer) const;
    std::vector<winppy::Packet> CreateGameRoomPlayerPacketsExcept(const Player* pPlayer) const;
    bool ChangePlayerState(uint32_t accountId, PlayerState newState);
    bool ChangePlayerTeam(uint32_t accountId, GameTeam newTeam);
private:
    bool IsFull() const;
    bool IsHost(const Player* pPlayer) const { return m_pHost == pPlayer; }
    bool FindPlayer(uint32_t accountId, GameTeam& team, size_t& index, PlayerState& state) const;
    const Player* FindNewHost() const;
private:
    uint64_t m_id;
    uint16_t m_no;

    GameRoomTeamFormat m_tf;
    GameMap m_map;
    GameRoomState m_state;

    std::wstring m_name;
private:
    std::vector<GameRoomPlayer> m_redTeam;
    std::vector<GameRoomPlayer> m_blueTeam;
    const Player* m_pHost;
};
