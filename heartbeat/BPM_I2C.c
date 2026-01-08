#include <libpynq.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../utils.h"
#define REFRESH_USEC 4000000

void display1(int beats, uint32_t bpm){

    unsigned char BEATS[] = "Beats: ";
    unsigned char BEPM[] = "BPM: ";
    char BeatArray[5];
    char BPMArray[5];
    sprintf(BeatArray, "%d", beats);
    sprintf(BPMArray, "%d", bpm);
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

    // init for slave code
    const uint32_t my_slave_address = 0x70; // Other slave addresses: 0x80, 0x90
    uint32_t my_register_map[32] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    const uint32_t my_register_map_length = sizeof(my_register_map)/sizeof(uint32_t);

    iic_reset(IIC0);
    iic_set_slave_mode(IIC0, my_slave_address, &(my_register_map[0]), my_register_map_length);
    iic_slave_mode_handler(IIC0);
    __clock_t t = clock() - REFRESH_USEC;

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
        uint32_t bps = 0.0;   // beats per second
        uint32_t bpm = 0.0;   // beats per minute
        if (elapsed > 0.0) {
            bps = beats / elapsed;
            bpm = bps * 60.0;
        }

        printf("Beats: %d | Time: %.2f s | Rate: %d beats/s (%d bpm)\n", beats, elapsed, bps, bpm);

        iic_slave_mode_handler(IIC0);
        if (clock() - t > REFRESH_USEC) {
            display1(beats, bpm);
            t = clock();

            // Generate a 4 character code
            unsigned char buffer[5];
            buffer[4] = '\0';
            unsigned char str[] = "Sent:";
            for (int i = 0; i < 4; i++) buffer[i] = 48 + rand() % 10;

            //Encode it into a single 32bit int
            char_to_int32(buffer, &bpm);
            
            my_register_map[5] = bpm; // set value
            my_register_map[0] = 0; // tell master that they've not read the value

            // Print data to console and to display
            printf("Sent %s\n",buffer);
            displayFillScreen(&display, RGB_BLACK);
            displayDrawString(&display, fx16G, 160, 84, str, RGB_GREEN);
            displayDrawString(&display, fx16G, 160, 100, buffer, RGB_GREEN);

            // Required for printing within the loop
            fflush(stdout); 
            }
    }

    

    destroy();
    return EXIT_SUCCESS;
}

