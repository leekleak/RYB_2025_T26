#include <libpynq.h>
#include <time.h>
#include "../utils.h"

#define REFRESH_USEC 4000000

int main(void)
{
  init();
  const uint32_t my_slave_address = 0x70;
  uint32_t my_register_map[32] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  const uint32_t my_register_map_length = sizeof(my_register_map)/sizeof(uint32_t);

  iic_reset(IIC0);
  iic_set_slave_mode(IIC0, my_slave_address, &(my_register_map[0]), my_register_map_length);
  iic_slave_mode_handler(IIC0);
  __clock_t t = clock() - REFRESH_USEC;

  while (1) {
    iic_slave_mode_handler(IIC0);
    
    if (clock() - t > REFRESH_USEC) {
      t = clock();

      // Generate a 4 character code
      unsigned char buffer[5];
      buffer[4] = '\0';
      unsigned char str[] = "Sent:";
      uint32_t rando = 0;
      for (int i = 0; i < 4; i++) buffer[i] = 48 + rand() % 10;

      //Encode it into a single 32bit int
      char_to_int32(buffer, &rando);
      
      my_register_map[5] = rando; // set value
      my_register_map[0] = 0; // tell master that they've not read the value

      // Print data to console and to display
      printf("Sent %s\n",buffer);
      displayFillScreen(&display, RGB_BLACK);
      displayDrawString(&display, fx16G, 100, 84, str, RGB_GREEN);
      displayDrawString(&display, fx16G, 100, 100, buffer, RGB_GREEN);

      // Required for printing within the loop
      fflush(stdout); 
    }
  }

  destroy();
  return EXIT_SUCCESS;
}