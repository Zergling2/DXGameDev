#pragma once

#include <cstddef>
#include <cstdint>

constexpr size_t MAX_GAME_PER_LIST_PAGE = 12;
constexpr size_t MAX_PLAYERS_PER_TEAM = 8;
constexpr size_t CHAT_MSG_ITEM_ROW_COUNT = 7;
constexpr size_t MAX_CHAT_MSG_LEN = 56;
constexpr size_t MAX_GAME_ROOM_NAME_LEN = 24;
constexpr size_t MAX_ID_LEN = 16;
constexpr size_t MIN_ID_LEN = 6;
constexpr size_t MAX_PW_LEN = 16;
constexpr size_t MIN_PW_LEN = 8;
constexpr size_t MAX_NICKNAME_LEN = 12;

constexpr uint32_t HEADER_CODE = 0xbeef9999;
constexpr uint16_t SASERVER_PORT = 37015;
constexpr size_t CHANNEL_COUNT = 6;
constexpr size_t MAX_SESSIONS_PER_CHANNEL = 500;