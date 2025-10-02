#include <libpynq.h>
#include <time.h>

int main(void)
{
  //switchbox_set_pin(IO_A0, SWB_IIC0_SCL);
  //switchbox_set_pin(IO_A1, SWB_IIC0_SDA);
  // this is the address by which this slave is reached by the master
  // different slaves must have different addresses
  const uint32_t my_slave_address = 0x70;
  // this array contains 32 registers that can be written & read by the master
  // the slave can of course modify the values of the registers
  uint32_t my_register_map[32] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  const uint32_t my_register_map_length = sizeof(my_register_map)/sizeof(uint32_t);

  pynq_init();
  switchbox_set_pin(IO_PMODA3, SWB_IIC0_SCL);
  switchbox_set_pin(IO_PMODA4, SWB_IIC0_SDA);
  iic_init(IIC0);
  iic_reset(IIC0);
  iic_set_slave_mode(IIC0, my_slave_address, &(my_register_map[0]), my_register_map_length);
  iic_slave_mode_handler(IIC0);
  __clock_t t = clock();
  while (1) {
    iic_slave_mode_handler(IIC0);
    
    if (clock() - t > 500000) {
      t = clock();
      my_register_map[5] = rand(); // set value
      my_register_map[0] = 0; // tell master that they've not read the value    
      //printf("Sent %d", my_register_map[5]);
    }
  }
  iic_destroy(IIC0);
  pynq_destroy();
  return EXIT_SUCCESS;
}