#include <libpynq.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../utils.h"


void display1(int beats, double bpm){

    unsigned char BEATS[] = "Beats: ";
    unsigned char BEPM[] = "BPM: ";
    char BeatArray[5];
    char BPMArray[5];
    sprintf(BeatArray, "%d", beats);
    sprintf(BPMArray, "%.1f", bpm);
    displayFillScreen(&display, RGB_BLACK);
    displayDrawString(&display, fx16G, 30, 84, BEATS  , RGB_RED);
    displayDrawString(&display, fx16G, 100, 84, (uint8_t *)BeatArray, RGB_GREEN);
    displayDrawString(&display, fx16G, 30, 120, BEPM, RGB_RED);
    displayDrawString(&display, fx16G, 100, 120, (uint8_t *)BPMArray, RGB_GREEN);    
    sleep_msec(150);      

}

int main(void) {
    init();
    adc_init();

    // adding the 5V output
    gpio_set_direction(IO_AR2, GPIO_DIR_OUTPUT);
    gpio_set_level(IO_AR2, GPIO_LEVEL_HIGH);



    double volts;
    int beat;
    int beats = 0;
    int sawAzero = 0;

    // take start time
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while (1) {

        volts = adc_read_channel(ADC5);
        printf("voltage: %f \n", volts);

        if (volts < 0.8) {
            beat = 1;
        } else {
            beat = 0;
        }
        printf("Beat: %d \n", beat);

        if (beat == 0) {
            sawAzero = 1;
        }
        if (beat == 1 && sawAzero == 1) {
            beats += 1;
            sawAzero = 0;
        }

        // get current time
        clock_gettime(CLOCK_MONOTONIC, &now);

        // calculate elapsed time in seconds (as double)
        double elapsed = (now.tv_sec - start.tv_sec)
                       + (now.tv_nsec - start.tv_nsec) / 1e9;

        // avoid division by zero at the very beginning
        double bps = 0.0;   // beats per second
        double bpm = 0.0;   // beats per minute
        if (elapsed > 0.0) {
            bps = beats / elapsed;
            bpm = bps * 60.0;
        }

        printf("Beats: %d | Time: %.2f s | Rate: %.3f beats/s (%.1f bpm)\n", beats, elapsed, bps, bpm);
        display1(beats, bpm);


    }

    destroy();
    return EXIT_SUCCESS;
}

