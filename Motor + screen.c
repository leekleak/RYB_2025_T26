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

    int f=100000, a=1000000, df, da, s=1, sa;
    float sf;

    const uint32_t my_slave_address = 0x72; // Other slave addresses: 0x80, 0x90
    uint32_t my_register_map[32] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    const uint32_t my_register_map_length = sizeof(my_register_map)/sizeof(uint32_t);

    iic_reset(IIC0);
    iic_set_slave_mode(IIC0, my_slave_address, &(my_register_map[0]), my_register_map_length);
    iic_slave_mode_handler(IIC0);
    __clock_t t = clock() - REFRESH_USEC;

    while(true){
      iic_slave_mode_handler(IIC0);


      if (my_register_map[0] == 0) { // New instructions from master
        if(s==0){
          df = 0.95*f;
          da = 0.95*a;
          sf = 0.2;
          sa = 20;
      } else if(s==1){
          df = 0.80*f;
          da = 0.80*a;
          sf = 0.35;
          sa = 40;
      } else if(s==2){
          df = 0.60*f;
          da = 0.60*a;
          sf = 0.5;
          sa = 60;
      } else if(s==3){
          df = 0.40*f;
          da = 0.40*a;
          sf = 0.65;
          sa = 80;
      } else if(s==4){
          df = 0.20*f;
          da = 0.20*a;
          sf = 0.70;
          sa = 100;
      } else{
          printf("Error motor driver.");
          df = f;
          da = a;
      }

    
      //frequency
      switchbox_set_pin(IO_AR0, SWB_PWM0); //Set PWM0 to arduino pin 0
      pwm_init(PWM0,f);
      pwm_set_duty_cycle(PWM0, df);
      pwm_set_steps(PWM0, 100000); 
      //amplitude
      switchbox_set_pin(IO_AR1, SWB_PWM1); //Set PWM0 to arduino pin 1
      pwm_init(PWM1,a);
      pwm_set_duty_cycle(PWM1, da);
      pwm_set_steps(PWM1, 1000000); 
      }
      
      if (clock() - t > REFRESH_USEC) {
        t = clock();
        display1(sa, sf);
      }
    }


    pwm_destroy(PWM0);
    pwm_destroy(PWM1);
    destroy();

}
