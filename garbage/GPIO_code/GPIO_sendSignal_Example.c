#include <libpynq.h>

int main (void)
{
  pynq_init();
  gpio_init();

  // Configure AR2 as output pin
  gpio_set_direction(IO_AR2, GPIO_DIR_OUTPUT);

  // Send signal pattern: toggle between HIGH and LOW
  while(1) {
    printf("Sending HIGH on AR2\n");
    gpio_set_level(IO_AR2, GPIO_LEVEL_HIGH);
    sleep_msec(1000);
    gpio_set_level(IO_AR2, GPIO_LEVEL_LOW);
    sleep_msec(1000);
  }
  gpio_destroy();
  pynq_destroy();
  return EXIT_SUCCESS;
}