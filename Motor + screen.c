#include <libpynq.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../utils.h"

#define REFRESH_USEC 1000000

void display1(int fre, int amp){
    unsigned char AMP[] = "Amplitude: ";
    unsigned char FRE[] = "Frequency: ";
    char AmpArray[4];
    char FreArray[4];
    sprintf(AmpArray, "%d", amp);
    sprintf(FreArray, "%d", fre);
    displayFillScreen(&display, RGB_BLACK);
    displayDrawString(&display, fx16G, 30, 84, AMP, RGB_RED);
    displayDrawString(&display, fx16G, 120, 84, (uint8_t *)AmpArray, RGB_GREEN);
    displayDrawString(&display, fx16G, 30, 120, FRE, RGB_RED);
    displayDrawString(&display, fx16G, 120, 120, (uint8_t *)FreArray, RGB_GREEN);
}

int main(void){
    init();
    switchbox_set_pin(IO_AR0, SWB_PWM0);
    switchbox_set_pin(IO_AR1, SWB_PWM1);

    int f=100000, a=100000, df, da;
    pwm_init(PWM0,f);
    pwm_init(PWM1,a);
    int freq, amp;

    const uint32_t my_slave_address = 0x72;
    uint32_t my_register_map[32] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    const uint32_t my_register_map_length = sizeof(my_register_map)/sizeof(uint32_t);

    iic_reset(IIC0);
    iic_set_slave_mode(IIC0, my_slave_address, &(my_register_map[0]), my_register_map_length);
    iic_slave_mode_handler(IIC0);
    __clock_t t = clock() - REFRESH_USEC;
    while(1){
        iic_slave_mode_handler(IIC0);
        if (my_register_map[0] == 0) { // New instructions from master
            // master will send from 0 to 4
            /*
            4 - 100%
            3 - 80%
            2 - 60%
            1 - 40 %
            0 - 20%
            */
            df = my_register_map[1];
            da = my_register_map[2];
            printf("Recieved: freq = %d, amp = %d \n", df, da);
            fflush(stdout); // idk if needed, just putting it to be sure.
            freq = (1 - (df+1)*0.2)*f; 
            amp = (1- (da+1)*0.2)*a;
            // if df or da=4, than from the lines above the freq/amp would be 0, because 1-1 = 0
            if (df == 4 ){
                freq = 0.05*f; // setting it to 95% speed.
            }
            if (da == 4){
                amp = 0.05*a;
            }
            my_register_map[0] = 1;

        }
        if (clock() - t > REFRESH_USEC){ // this is maybe not the correct way of doing it.
            t = clock();
            display1(df, da);
        
            //frequency
            pwm_set_duty_cycle(PWM0, freq);
            pwm_set_steps(PWM0, -1); 

            //amplitude
            pwm_set_duty_cycle(PWM1, amp);
            pwm_set_steps(PWM1, -1); 
        }
    }


    pwm_destroy(PWM0);
    pwm_destroy(PWM1);
    destroy();

}