#pragma once

#include <vector>
#include <winppy/Network/TCPServer.h>
#include <winppy/Network/Packet.h>
#include "Constants.h"
#include "Contents.h"

enum class HostGameStartableResult
{
    AlreadyStarted,
    Startable,
    NotReady,

    Unknown
};

class Player;

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

    GameRoomTeamFormat GetTeamFormat() const { return m_tf; }
    void SetTeamFormat(GameRoomTeamFormat tf) { m_tf = tf; }

    GameMap GetMap() const { return m_map; }
    void SetMap(GameMap map) { m_map = map; }

    GameRoomState GetState() const { return m_state; }
    void SetState(GameRoomState state) { m_state = state; }

    const std::wstring& GetName() const { return m_name; }
    void SetName(const wchar_t* name) { m_name = name; }

    void AddPlayerAsHost(winppy::TCPServer& server, Player* pPlayer);
    bool AddPlayer(winppy::TCPServer& server, Player* pPlayer);
    size_t RemovePlayer(winppy::TCPServer& server, Player* pPlayer);
    const Player* GetHost() const { return m_pHost; }

    HostGameStartableResult IsGameStartable() const;
    void ChangeReadyPlayersAndHostStateToPlaying(winppy::TCPServer& server);
    size_t GetNumOfPlayers() const { return m_redTeam.size() + m_blueTeam.size(); }
    void ChangePlayerState(winppy::TCPServer& server, uint32_t accountId, PlayerState newState);
    PlayerState GetPlayerState(uint32_t accountId) const;
    bool ChangePlayerTeam(winppy::TCPServer& server, uint32_t accountId, GameTeam newTeam);

    bool IsFull() const;

    void BroadcastPacket(winppy::TCPServer& server, winppy::Packet packet) const;
    void BroadcastPacketExcept(winppy::TCPServer& server, winppy::Packet packet, uint32_t exceptorAccountId) const;
private:
    bool IsHost(const Player* pPlayer) const { return m_pHost == pPlayer; }
    bool FindPlayer(uint32_t accountId, GameTeam& team, size_t& index, PlayerState& state) const;
    const Player* SelectRandomPlayer() const;
private:
    uint64_t m_id;
    uint16_t m_no;
    GameRoomState m_state;
    GameRoomTeamFormat m_tf;
    GameMap m_map;
    std::wstring m_name;
    std::vector<GameRoomPlayer> m_redTeam;
    std::vector<GameRoomPlayer> m_blueTeam;
    const Player* m_pHost;
};
