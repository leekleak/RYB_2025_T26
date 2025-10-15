#include <stdio.h>
#include <stdint.h>

display_t display;
FontxFile fx16G[2];

void init() {
    pynq_init();
    switchbox_set_pin(IO_PMODA3, SWB_IIC0_SCL);
    switchbox_set_pin(IO_PMODA4, SWB_IIC0_SDA);
    iic_init(IIC0);

    // Display shit
    display_init(&display);
    display_set_flip(&display, true, true);
    displayFillScreen(&display, RGB_BLACK);

    uint8_t buffer_fx16G[FontxGlyphBufSize];
    uint8_t fontWidth_fx16G, fontHeight_fx16G;
    InitFontx(fx16G, "/boot/ILGH16XB.FNT", "");
    GetFontx(fx16G, 0, buffer_fx16G, &fontWidth_fx16G, &fontHeight_fx16G);
}

void destroy() {
    iic_destroy(IIC0);
    display_destroy(&display);
    pynq_destroy();
}

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