#pragma once

#include <cstdint>
#include <limits>
#include "Constants.h"

class Session;

class Player
{
public:
    Player(uint32_t accountId, uint16_t level, uint32_t point, const wchar_t* nickname);

    void BindSession(Session* pSession) { m_pSession = pSession; }
    void UnbindSession() { m_pSession = nullptr; }
    Session* GetSession() const { return m_pSession; }
    uint32_t GetAccountId() const { return m_accountId; }
    void MarkEnterChannel(uint8_t channelId) { m_inChannel = true; m_channelId = channelId; }
    void MarkExitChannel() { m_inChannel = false; m_channelId = (std::numeric_limits<uint8_t>::max)(); }
    bool IsInChannel() const { return m_inChannel; }
    uint8_t GetChannelId() const { return m_channelId; }
    void MarkEnterRoom(uint64_t roomId) { m_inRoom = true; m_roomId = roomId; }
    void MarkExitRoom() { m_inRoom = false; m_roomId = (std::numeric_limits<uint64_t>::max)(); }
    bool IsInRoom() const { return m_inRoom; }
    uint64_t GetRoomId() const { return m_roomId; }
    void SetNickname(const wchar_t* nickname);
    uint16_t GetNicknameLen() const { return m_nicknameLen; }
    const wchar_t* GetNickname() const { return m_nickname; }
    void SetLevel(uint16_t level) { m_level = level; }
    uint16_t GetLevel() const { return m_level; }
    void SetPoint(uint32_t point) { m_point = point; }
    uint32_t GetPoint() const { return m_point; }
private:
    Session* m_pSession;
    uint32_t m_accountId;
    bool m_inChannel;
    uint8_t m_channelId;
    bool m_inRoom;
    uint64_t m_roomId;
    uint16_t m_nicknameLen;
    wchar_t m_nickname[MAX_NICKNAME_LEN + 1];
    uint16_t m_level;
    uint32_t m_point;
};
