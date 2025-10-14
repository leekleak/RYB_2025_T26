#include <libpynq.h>
#include <time.h>
#include "../utils.h"

int main(void)
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


  // this is the address by which this slave is reached by the master
  // different slaves must have different addresses
  const uint32_t my_slave_address = 0x70;
  // this array contains 32 registers that can be written & read by the master
  // the slave can of course modify the values of the registers
  uint32_t my_register_map[32] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  const uint32_t my_register_map_length = sizeof(my_register_map)/sizeof(uint32_t);

  iic_reset(IIC0);
  iic_set_slave_mode(IIC0, my_slave_address, &(my_register_map[0]), my_register_map_length);
  iic_slave_mode_handler(IIC0);
  __clock_t t = clock();
  while (1) {
    iic_slave_mode_handler(IIC0);
    
    if (clock() - t > 500000) {
      t = clock();


      unsigned char buffer[5];
      uint32_t rando = 0;
      for (int i = 0; i < 4; i++) buffer[i] = 48 + rand() % 10;
      char_to_int32(buffer, &rando);
      
      my_register_map[5] = rando; // set value
      my_register_map[0] = 0; // tell master that they've not read the value    

      displayFillScreen(&display, RGB_BLACK);
      //int32_to_char(my_register_map[5], buffer);

      printf("Sent %s\n",buffer);
      displayDrawString(&display, fx16G, 100, 100, buffer, RGB_GREEN);
      fflush(stdout);
    }
  }
  iic_destroy(IIC0);
  pynq_destroy();
  return EXIT_SUCCESS;
}