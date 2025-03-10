#include "protocol_header.hpp"
#include <algorithm>

namespace {
constexpr uint8_t HEADER_1 = 0xeb;
constexpr uint8_t HEADER_2 = 0x90;
constexpr uint8_t HEADER_3 = 0xeb;
constexpr uint8_t HEADER_4 = 0x90;
constexpr uint8_t RESERVED_VALUE = 0x00;

bool isLittleEndian() {
    static const uint16_t value = 0x0001;
    return *reinterpret_cast<const uint8_t*>(&value) == 0x01;
}

void toLittleEndian(char* buf, size_t size) {
    if (!isLittleEndian()) {
        for (size_t i = 0; i < size / 2; ++i) {
            std::swap(buf[i], buf[size - i - 1]);
        }
    }
}
}  // namespace

namespace protocol {

ProtocolHeader::ProtocolHeader()
    : sync_byte1(HEADER_1), sync_byte2(HEADER_2), sync_byte3(HEADER_3), sync_byte4(HEADER_4), length(0), sequenceNumber(0) {
    reserved.fill(RESERVED_VALUE);
}

ProtocolHeader::ProtocolHeader(uint16_t length, uint16_t sequenceNumber)
    : sync_byte1(HEADER_1),
      sync_byte2(HEADER_2),
      sync_byte3(HEADER_3),
      sync_byte4(HEADER_4),
      length(length),
      sequenceNumber(sequenceNumber) {
    reserved.fill(RESERVED_VALUE);
    toLittleEndian(reinterpret_cast<char*>(&this->length), sizeof(this->length));
}

bool ProtocolHeader::validateSyncBytes() const {
    return sync_byte1 == HEADER_1 && sync_byte2 == HEADER_2 && sync_byte3 == HEADER_3 && sync_byte4 == HEADER_4;
}

uint16_t ProtocolHeader::getBodySize() const {
    return length;
}

}  // namespace protocol
