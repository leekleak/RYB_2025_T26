#include <libpynq.h>

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