#include <libpynq.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../utils.h"

#define REFRESH_USEC 1000000

void display1(int amp, float fre){

    unsigned char AMP[] = "Amplitude: ";
    unsigned char FRE[] = "Frequency: ";
    char AmpArray[5];
    char FreArray[5];
    sprintf(AmpArray, "%d", amp);
    sprintf(FreArray, "%.2f", fre);
    displayFillScreen(&display, RGB_BLACK);
    displayDrawString(&display, fx16G, 30, 84, AMP  , RGB_RED);
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

    const uint32_t my_slave_address = 0x72; // Other slave addresses: 0x80, 0x90
    uint32_t my_register_map[32] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    const uint32_t my_register_map_length = sizeof(my_register_map)/sizeof(uint32_t);


    iic_reset(IIC0);
    iic_set_slave_mode(IIC0, my_slave_address, &(my_register_map[0]), my_register_map_length);
    iic_slave_mode_handler(IIC0);
    __clock_t t = clock() - REFRESH_USEC;

    if (clock() - t > REFRESH_USEC){
      iic_slave_mode_handler(IIC0);


      if (my_register_map[0] == 0) { // New instructions from master
        // master will send from 0 to 40
        /*
         4 - 100%
         3 - 80%
         2 - 60%
         1 - 40 %
         0 - 20%
        
        */
            df = my_register_map[1];
            da = my_register_map[2];
            df = (df+1)*0.2;
            da = (da+1)*0.2;
            freq = (1 - df)*f;
            amp = (1- da)*a;
            my_register_map[0] = 1;
      }

      display1(df, da);
      //frequency
      // Set PWM0 to arduino pin 0
      
      pwm_set_duty_cycle(PWM0, freq);
      pwm_set_steps(PWM0, -1); 
      //amplitude
       //Set PWM0 to arduino pin 1
      
      pwm_set_duty_cycle(PWM1, amp);
      pwm_set_steps(PWM1, -1); 
      }
      


    pwm_destroy(PWM0);
    pwm_destroy(PWM1);
    destroy();

}