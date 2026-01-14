#include <libpynq.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../utils.h"
#define REFRESH_USEC 2000000
#define MAX_BPM_SAMPLES 10  // Store last 10 BPM readings (smaller = more responsive)

void display1(int beats, uint32_t bpm){
    unsigned char BEATS[] = "Beats: ";
    unsigned char BEPM[] = "BPM: ";
    char BeatArray[5];
    char BPMArray[5];
    sprintf(BeatArray, "%d", beats);
    sprintf(BPMArray, "%d", bpm);
    displayFillScreen(&display, RGB_BLACK);
    displayDrawString(&display, fx16G, 30, 84, BEATS, RGB_RED);
    displayDrawString(&display, fx16G, 100, 84, (uint8_t *)BeatArray, RGB_GREEN);
    displayDrawString(&display, fx16G, 30, 120, BEPM, RGB_RED);
    displayDrawString(&display, fx16G, 100, 120, (uint8_t *)BPMArray, RGB_GREEN);        
}

// Function to calculate mode of BPM values
uint32_t calculate_mode(uint32_t *bpm_array, int count) {
    if (count == 0) return 0;
    if (count == 1) return bpm_array[0];
    
    uint32_t mode = bpm_array[0];
    int max_count = 1;
    
    // Check each value
    for (int i = 0; i < count; i++) {
        int current_count = 1;
        
        // Count how many times this value appears
        for (int j = i + 1; j < count; j++) {
            if (bpm_array[j] == bpm_array[i]) {
                current_count++;
            }
        }
        
        // Update mode if this value appears more often
        if (current_count > max_count) {
            max_count = current_count;
            mode = bpm_array[i];
        }
    }
    
    return mode;
}

int main(void) {
    init();
    adc_init();

    // adding the 5V output
    gpio_set_direction(IO_AR6, GPIO_DIR_OUTPUT);
    gpio_set_level(IO_AR6, GPIO_LEVEL_HIGH);

    // init for slave code
    const uint32_t my_slave_address = 0x71;
    uint32_t my_register_map[32] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    const uint32_t my_register_map_length = sizeof(my_register_map)/sizeof(uint32_t);

    // Array to store BPM readings
    uint32_t bpm_samples[MAX_BPM_SAMPLES];
    int sample_count = 0;

    iic_reset(IIC0);
    iic_set_slave_mode(IIC0, my_slave_address, &(my_register_map[0]), my_register_map_length);
    iic_slave_mode_handler(IIC0);

    double volts;
    int beat;
    int beats = 0;
    int sawAzero = 0;

    // take start time
    struct timespec tt1, tt2, looper;
    clock_gettime(CLOCK_MONOTONIC, &tt1);
    clock_gettime(CLOCK_MONOTONIC, &tt2);
    clock_gettime(CLOCK_MONOTONIC, &looper);
    double elapsed2 = 0;

    double bps = 0.0;   // beats per second

    while (1) {
        volts = adc_read_channel(ADC5);

        beat = (volts < 1.0);

        if (beat == 0) {
            sawAzero = 1;
        }
        if (beat == 1 && sawAzero == 1) {
            clock_gettime(CLOCK_MONOTONIC, &tt2);
            elapsed2 = (tt2.tv_sec - tt1.tv_sec)
                       + (tt2.tv_nsec - tt1.tv_nsec) / 1e9;
            sawAzero = 0;

            if (elapsed2 > 0.02) {
                printf("Time: %.2f\n", elapsed2);
                tt1 = tt2;
                
                // Calculate individual beat BPM and store it
                uint32_t beat_bpm = (uint32_t)(60.0 / elapsed2);
                
                // Add each beat's BPM to samples array
                bpm_samples[sample_count % MAX_BPM_SAMPLES] = beat_bpm;
                sample_count++;
                
                bps += elapsed2;
                beats += 1;
            }
        }

        iic_slave_mode_handler(IIC0);
        // take start time
        struct timespec current;
        clock_gettime(CLOCK_MONOTONIC, &current);
        int diff = (current.tv_sec - looper.tv_sec) * 1000000
                    + (current.tv_nsec - looper.tv_nsec) / 1e3;
                    
        if (diff > REFRESH_USEC) {
            clock_gettime(CLOCK_MONOTONIC, &looper);
            
            if (sample_count > 0) {
                // Calculate mode from stored samples
                int samples_to_use = (sample_count < MAX_BPM_SAMPLES) ? sample_count : MAX_BPM_SAMPLES;
                uint32_t bpm_mode = calculate_mode(bpm_samples, samples_to_use);
                
                printf("Beats: %d | Samples collected: %d | Mode BPM: %d\n", 
                       beats, samples_to_use, bpm_mode);
                
                display1(beats, bpm_mode);

                // Generate a 4 character code
                char buffer[5];
                buffer[4] = '\0';
                unsigned char str[] = "Sent:";
                sprintf(buffer, "%04d", bpm_mode);  // Encode mode BPM value
                
                my_register_map[5] = bpm_mode; // set value
                my_register_map[0] = 0; // tell master that they've not read the value

                // Print data to console and to display
                displayDrawString(&display, fx16G, 30, 150, str, RGB_RED);
                displayDrawString(&display, fx16G, 100, 150, (uint8_t *) buffer, RGB_GREEN);

                // Required for printing within the loop
                fflush(stdout);
            }
            
            bps = 0;
            beats = 0;
        }
        sleep_msec(10);
    }

    destroy();
    return EXIT_SUCCESS;
}