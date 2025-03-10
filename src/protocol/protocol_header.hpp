#pragma once
#include <array>
#include <cstdint>

namespace protocol {

#pragma pack(push, 1)
struct ProtocolHeader {
    uint8_t sync_byte1;
    uint8_t sync_byte2;
    uint8_t sync_byte3;
    uint8_t sync_byte4;
    uint16_t length;
    uint16_t sequenceNumber;
    std::array<uint8_t, 8> reserved;

    explicit ProtocolHeader();
    explicit ProtocolHeader(uint16_t length, uint16_t sequenceNumber);

    bool validateSyncBytes() const;
    uint16_t getBodySize() const;
};
#pragma pack(pop)

}  // namespace protocol
