#include <libpynq.h>
#include <stdio.h>
int main (void)
{
  // initialise all I/O
  pynq_init();
  iic_init(IIC0);
  uint32_t i;
  int reg = 1; // check the 2nd value in Slaves array
  // you can use multiple slaves, here only one is shown
  uint32_t slave_address = 0x70;
    uint32_t slave_address2 = 0x71;

  // read out all registers of the slave
  if ((iic_read_register(IIC0, slave_address, 1, (uint8_t *) &i, 4))){
    printf("reg[%d]=error - slave 1", reg);
  }else{
    printf("re[%d]=%d - slave 1", reg, i);
  }
  if ((iic_read_register(IIC0, slave_address2, 1, (uint8_t *) &i, 4))){
    printf("reg[%d]=error - slave 2", reg);
  }else{
    printf("re[%d]=%d - slave 2", reg, i);
  }
  // clean up after use
  pynq_destroy();
  return EXIT_SUCCESS;
}