#include <libpynq.h>
#include "../utils.h"

int main (void)
{
  init();
  uint32_t i;
  uint32_t slave_address = 0x70;
  
  while (1) {
    for (int reg=0; reg < 32; reg++) {

      // Failed to read register
      if (iic_read_register(IIC0, slave_address, reg, (uint8_t *) &i, 4)) { // 4 bytes
        printf("register[%d]=error\n",reg);
        displayDrawString(&display, fx16G, 100, 100, (uint8_t *)"error", RGB_GREEN);
        break;
      }
      
      // There's no fresh data to read; Loop again
      else if (reg == 0 && i == 1) {
        break;
      } 
      

      else {

        // If reg = 0, the data is fresh and we should read it
        if (reg == 0) {
          uint8_t tt = 1;
          uint8_t *t = &tt;
          iic_write_register(IIC0, slave_address, reg, t, 4);
        }

        // Our chosen register for testing
        if (reg == 5){
          printf("register[%d]=%d\n",reg,i);

          // Convert to char buffer and display
          unsigned char buffer[4];
          int32_to_char(i, buffer);
          displayDrawString(&display, fx16G, 100, 100, (uint8_t *)buffer, RGB_GREEN);
        }
      }
    }
  }

  destroy();
  return EXIT_SUCCESS;
}
