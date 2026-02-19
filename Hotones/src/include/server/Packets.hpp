#pragma once
#include <cstdint>

namespace Hotones::Net {

// ─── Packet type IDs ─────────────────────────────────────────────────────────
enum class PacketType : uint8_t {
    CONNECT       = 0x01, // Client → Server: request to join
    CONNECT_ACK   = 0x02, // Server → Client: assign ID & accept
    DISCONNECT    = 0x03, // Either direction: graceful leave
    PLAYER_UPDATE = 0x10, // Client → Server own state; Server → All clients
    PING          = 0x20,
    PONG          = 0x21,
    // ── Server-info query (no connection needed) ──────────────────────────
    SERVER_INFO_REQ  = 0x30, // Anyone → Server: request server info
    SERVER_INFO_RESP = 0x31, // Server → requester: server info response
};

// ─── Packet structures (no padding) ──────────────────────────────────────────
#pragma pack(push, 1)

struct PacketHeader {
    PacketType type;
    uint8_t    playerId; // sender's ID (0 = unassigned / server)
};

// Client → Server: join request
struct ConnectPacket {
    PacketHeader header;   // type = CONNECT, playerId = 0
    char         name[16]; // null-terminated display name
};

// Server → Client: join accepted
struct ConnectAckPacket {
    PacketHeader header;     // type = CONNECT_ACK, playerId = assigned ID
    uint8_t      assignedId; // mirrors header.playerId for clarity
};

// Either direction: graceful leave
struct DisconnectPacket {
    PacketHeader header; // type = DISCONNECT, playerId = who left
};

// Position/rotation snapshot (client → server, or server broadcast to all)
struct PlayerUpdatePacket {
    PacketHeader header; // type = PLAYER_UPDATE, playerId = whose state
    float        posX, posY, posZ;
    float        rotX, rotY; // yaw, pitch
};

struct PingPacket {
    PacketHeader header;
    uint32_t     seq;
};

// ── Server-info query (fire-and-forget, no prior connection required) ─────────

// Anyone → Server: just the header, no extra payload
struct ServerInfoReqPacket {
    PacketHeader header; // type = SERVER_INFO_REQ, playerId = 0
};

// Server → requester: advertise current state
struct ServerInfoRespPacket {
    PacketHeader header;        // type = SERVER_INFO_RESP, playerId = 0
    uint8_t      playerCount;   // active connected players
    uint8_t      maxPlayers;    // maximum allowed
    uint16_t     port;          // bound port (mirrors what was queried)
    char         pakName[32];   // pack display name, empty = no pack loaded
    char         serverName[32];// optional server display name
};

#pragma pack(pop)

} // namespace Hotones::Net
