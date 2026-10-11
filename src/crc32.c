#include "crc32.h"

uint32_t crc32_compute(const void *data, size_t length)
{
    const uint8_t *bytes = (const uint8_t *)data;
    uint32_t crc = 0xFFFFFFFFu;

    if (data == NULL && length != 0U) {
        return 0U;
    }

    for (size_t i = 0U; i < length; ++i) {
        crc ^= bytes[i];
        for (unsigned bit = 0U; bit < 8U; ++bit) {
            const uint32_t mask = (uint32_t)(-(int32_t)(crc & 1U));
            crc = (crc >> 1U) ^ (0xEDB88320u & mask);
        }
    }
    return ~crc;
}
