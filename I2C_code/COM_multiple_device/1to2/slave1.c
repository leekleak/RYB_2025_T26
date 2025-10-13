#include <libpynq.h>
int main(void)
{
  /*
  YOU can replace the number with "CRM" - it will still be 4 bytes
  
  */
  // this is the address by which this slave is reached by the master
  // different slaves must have different addresses
  const uint32_t my_slave_address = 0x70;
  // this array contains 32 registers that can be written & read by the master
  // the slave can of course modify the values of the registers
  uint32_t my_register_map[8] = {0,0,0,0,0,0,0,0};
  const uint32_t my_register_map_length = sizeof(my_register_mmap)/sizeof(uint32_t);
 
  pynq_init();
  iic_init(IIC0);
  iic_reset(IIC0);
  iic_set_slave_mode(IIC0, my_slave_address, &(my_register_map[0]), my_register_map_length);
  while (1) {
    // the slave mode handler must be run regularly to react to the master
    iic_slave_mode_handler(IIC0);
    // insert your own code here, to do whatever the slave needs to do;
    // make sure that you execute the slave mode handler regularly enough though
    sleep_msec(10);
  }
  iic_destroy(IIC0);
  pynq_destroy();
  return EXIT_SUCCESS;
}