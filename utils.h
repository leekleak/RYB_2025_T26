#include <stdio.h>
#include <stdint.h>

void int32_to_char(uint32_t in, unsigned char out[4]) {
    out[0] = (in >> 24) & 0xFF;
    out[1] = (in >> 16) & 0xFF;
    out[2] = (in >> 8)  & 0xFF;
    out[3] = in & 0xFF;
}

void char_to_int32(const unsigned char in[4], uint32_t *out) {
    *out = ((uint32_t)(uint8_t)in[0] << 24) |
           ((uint32_t)(uint8_t)in[1] << 16) |
           ((uint32_t)(uint8_t)in[2] << 8)  |
           ((uint32_t)(uint8_t)in[3]);
}