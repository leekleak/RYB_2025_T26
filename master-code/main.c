#include <libpynq.h>
#include "../utils.h"

int main (void)
{

  // initialise all I/O
  pynq_init();
  switchbox_set_pin(IO_PMODA3, SWB_IIC0_SCL);
  switchbox_set_pin(IO_PMODA4, SWB_IIC0_SDA);
  iic_init(IIC0);

  // Display shit

  display_t display;
  display_init(&display);
  display_set_flip(&display, true, true);
  displayFillScreen(&display, RGB_BLACK);

  uint8_t buffer_fx16G[FontxGlyphBufSize];
  uint8_t fontWidth_fx16G, fontHeight_fx16G;
  FontxFile fx16G[2];
  InitFontx(fx16G, "/boot/ILGH16XB.FNT", "");
  GetFontx(fx16G, 0, buffer_fx16G, &fontWidth_fx16G, &fontHeight_fx16G);


  // End Display shit


  uint32_t i;
  // you can use multiple slaves, here only one is shown
  uint32_t slave_address = 0x70;
  // read out all registers of the slave
  while(1){
    for (int reg=0; reg < 32; reg++) {
      if (iic_read_register(IIC0, slave_address, reg, (uint8_t *) &i, 4)) { // 4 bytes
        printf("register[%d]=error\n",reg);
        displayDrawString(&display, fx16G, 100, 100, (uint8_t *)"error", RGB_GREEN);
        break;
      } else if (reg == 0 && i == 1) {
        break;
      } else {
        if (reg == 0) {
          uint8_t tt = 1;
          uint8_t *t = &tt;
          iic_write_register(IIC0, slave_address, reg, t, 4);
        }
        if (reg == 5){
          printf("register[%d]=%d\n",reg,i);
          char buffer[4];
          int32_to_char(i, buffer);
          displayDrawString(&display, fx16G, 100, 100, (uint8_t *)buffer, RGB_GREEN);
        }
      }
    }
  }
  printf("Complated 1 loop");
  sleep_msec(10);
  //}
  // clean up after use
  pynq_destroy();
  return EXIT_SUCCESS;
}
