#include <libpynq.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    pynq_init();
    adc_init();

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

        printf("Beats: %d | Time: %.2f s | Rate: %.3f beats/s (%.1f bpm)\n",
               beats, elapsed, bps, bpm);

    }

    pynq_destroy();
    return EXIT_SUCCESS;
}
