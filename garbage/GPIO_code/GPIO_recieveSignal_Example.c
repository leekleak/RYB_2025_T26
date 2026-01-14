#include <libpynq.h>

int main (void)
{
  pynq_init();
  gpio_init();

  // Configure AR2 as output pin
  gpio_set_direction(IO_AR2, GPIO_DIR_INPUT);

  // Send signal pattern: toggle between HIGH and LOW
  while(1) {
    gpio_get_level(IO_AR0);
  }
  gpio_destroy();
  pynq_destroy();
  return EXIT_SUCCESS;
}