#include <libpynq.h>
#include <stdio.h>


int main(void) {
    pynq_init();
    adc_init();

    double volts;
    int beat;
    while(1){

    volts = adc_read_channel(ADC5);
    printf("voltage: %f \n", volts);
    if (volts < 0.5){
      beat = 1;
    } else {
      beat = 0;
    }
    printf("Beat: %d \n", beat);
    sleep_msec(100);
    }
  pynq_destroy();
  return EXIT_SUCCESS;
}