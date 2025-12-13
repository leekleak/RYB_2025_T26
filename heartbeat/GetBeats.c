#include <libpynq.h>
#include <stdio.h>


int main(void) {
    pynq_init();
    adc_init();

    double volts;
    int beat;
    int beats = 0;
    int sawAzero = 0;
    time_t start;
    while(1){

    volts = adc_read_channel(ADC5);
    printf("voltage: %f \n", volts);
    if (volts < 0.5){
      beat = 1;
    } else {
      beat = 0;
    }
    printf("Beat: %d \n", beat);

    if (beat == 0){
        sawAzero =1;
    }
    if (beat == 1 && sawAzero == 1){
        beats += 1;
        sawAzero = 0;
    }
    printf("&d", beats);
    sleep_msec(100);
    }
  pynq_destroy();
  return EXIT_SUCCESS;
}