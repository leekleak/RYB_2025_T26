#include <libpynq.h>
#include <time.h>
#include "../utils.h"

#define REFRESH_USEC 1000000

int main(void)
{
  init();
  const uint32_t my_slave_address = 0x99; // Other slave addresses: 0x80, 0x90
  uint32_t my_register_map[32] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  const uint32_t my_register_map_length = sizeof(my_register_map)/sizeof(uint32_t);

  iic_reset(IIC0);
  iic_set_slave_mode(IIC0, my_slave_address, &(my_register_map[0]), my_register_map_length);
  iic_slave_mode_handler(IIC0);
  __clock_t t = clock() - REFRESH_USEC;

  adc_init();
  double biggest = 0;
  double n = 0;
  double maximum = 3.3;

  while (1) {
    iic_slave_mode_handler(IIC0);
    
    unsigned char str[] = "Mic:";

    double input = adc_read_channel(ADC0);
    if (input > biggest) {
      biggest = input;
    }

    if (clock() - t > REFRESH_USEC) {
      t = clock();
    
      //printf("Samples: %1.0f\n", n);
      //printf("Average: %1.1f\n\n", biggest);
      
      if (maximum == 0) maximum = biggest;
      char charray[5];
      sprintf(charray, "%3.0f%", biggest / maximum * 100);
      
      // Print data to console and to display
      displayFillScreen(&display, RGB_BLACK);
      displayDrawString(&display, fx16G, 100, 84, str, RGB_GREEN);
      displayDrawString(&display, fx16G, 100, 100, charray, RGB_GREEN);

      // Required for printing within the loop
      fflush(stdout); 

      biggest = 0; 
      n = 0;
    }
  }

  destroy();
  return EXIT_SUCCESS;
}