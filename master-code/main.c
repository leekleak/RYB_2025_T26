#include <libpynq.h>
#include "../utils.h"

int main (void)
{
  init();
  uint32_t i;
  uint32_t slave_address1 = 0x70;
  uint32_t slave_address2 = 0x80;
  
  while (1) {

    /**
     * Slave 1
     */

    // Check if there's anything new in the registers
    if (iic_read_register(IIC0, slave_address1, 0, (uint8_t *) &i, 4)) { // 4 bytes
      printf("register[%d]=error\n", 0);
      displayFillScreen(&display, RGB_BLACK);
      displayDrawString(&display, fx16G, 100, 100, (uint8_t *)"error", RGB_GREEN);
      break;
    }
    if (i == 0) {
      uint8_t tt = 1;
      uint8_t *t = &tt;
      iic_write_register(IIC0, slave_address1, 0, t, 4);
      for (int reg=1; reg < 32; reg++) {
        iic_read_register(IIC0, slave_address1, reg, (uint8_t *) &i, 4);
        if (reg == 5){
          printf("register[%d]=%d\n",reg,i);
          // Convert to char buffer and display
          unsigned char buffer[4];
          int32_to_char(i, buffer);
          displayDrawFillRect(&display, 0, 0, 239, 120, RGB_BLACK);
          displayDrawString(&display, fx16G, 100, 100, (uint8_t *)"Output", RGB_GREEN);
          displayDrawString(&display, fx16G, 100, 116, (uint8_t *)buffer, RGB_GREEN);
        }
      }
    }

     /**
     * Slave 2
     */

    if (iic_read_register(IIC0, slave_address2, 0, (uint8_t *) &i, 4)) { // 4 bytes
      printf("register[%d]=error\n", 0);
      displayFillScreen(&display, RGB_BLACK);
      displayDrawString(&display, fx16G, 100, 100, (uint8_t *)"error", RGB_GREEN);
      break;
    }
    if (i == 0) {
      uint8_t tt = 1;
      uint8_t *t = &tt;
      iic_write_register(IIC0, slave_address2, 0, t, 4);
      for (int reg=1; reg < 32; reg++) {
        iic_read_register(IIC0, slave_address2, reg, (uint8_t *) &i, 4);
        if (reg == 5){
          printf("register[%d]=%d\n",reg,i);
          // Convert to char buffer and display
          unsigned char buffer[4];
          int32_to_char(i, buffer);
          displayDrawFillRect(&display, 0, 120, 239, 239, RGB_BLACK);
          displayDrawString(&display, fx16G, 100, 150, (uint8_t *)"Output", RGB_GREEN);
          displayDrawString(&display, fx16G, 100, 166, (uint8_t *)buffer, RGB_GREEN);
        }
      }
    }

    fflush(stdout);
  }

  destroy();
  return EXIT_SUCCESS;
}
