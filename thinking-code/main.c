#include <libpynq.h>
#include <time.h>
#include "../utils.h"
#define REFRESH_USEC 1000000

int main (void)
{
  init();
  uint32_t i;
  uint32_t slave_address1 = 0x70;
  uint32_t slave_address2 = 0x71;
  uint32_t slave_address3 = 0x72;
  
  int x_off[4] = {-1, 0, 1, 0};
  int y_off[4] = {0, -1, 0, 1};
  int coords_x = 4;
  int coords_y = 4;
  int progress = 0; // Neigbour checking progress

  int best_neighbour_stress = 100;
  int best_neighbour_i = 0;

  int cry = 100;
  int hb = 100;
  int stress = 100;

  __clock_t t = clock() - REFRESH_USEC;

  while (1) {
    /*
      Crying
    */
    /*if (iic_read_register(IIC0, slave_address1, 0, (uint8_t *) &i, 4)) {
      printf("register[%d]=cry\n", 0);
      displayFillScreen(&display, RGB_BLACK);
      displayDrawString(&display, fx16G, 100, 100, (uint8_t *)"error", RGB_GREEN);
      break;
    }
    if (i == 0) {
      uint8_t tt = 1;
      uint8_t *t = &tt;
      printf("cry\n");
      iic_write_register(IIC0, slave_address1, 0, t, 4);
      for (int reg=1; reg < 32; reg++) {
        iic_read_register(IIC0, slave_address1, reg, (uint8_t *) &i, 4);
        if (reg == 5){
          cry=i;
        }
      }
    }*/

    /*
      Heartbeat
    */
    /*int j = -1;
    if (iic_read_register(IIC0, slave_address2, 0, (uint8_t *) &j, 4)) {
      printf("register[%d]=hb\n", 0);
      displayFillScreen(&display, RGB_BLACK);
      displayDrawString(&display, fx16G, 100, 100, (uint8_t *)"error", RGB_GREEN);
      break;
    }
    if (j == 0) {
      uint8_t tt = 1;
      uint8_t *t = &tt;
      iic_write_register(IIC0, slave_address2, 0, t, 4);
      for (int reg=1; reg < 32; reg++) {
        iic_read_register(IIC0, slave_address2, reg, (uint8_t *) &j, 4);
        if (reg == 5){
          hb=i;
        }
      }
    }
    stress = hb / 2;
    if (stress > 100) stress = 100;
*/
    if (clock() - t > REFRESH_USEC) {
      t = clock();
      printf("Heartbeat %d, Crying: %d\n", hb, cry);

      if (progress < 4) {
        printf("Neighbour %d stress %d\n", progress, stress);
        if (stress < best_neighbour_stress) {
          best_neighbour_stress = stress;
          best_neighbour_i = progress;
        }

        int x1 = coords_x + x_off[progress];
        int y1 = coords_y + y_off[progress];
        int val = 0;
        printf("Asking motors to go to %d %d\n\n", x1, y1);
        /*iic_write_register(IIC0, slave_address3, 1, &x1, 4);
        iic_write_register(IIC0, slave_address3, 2, &y1, 4);
        iic_write_register(IIC0, slave_address3, 0, &val, 4);*/
        progress = progress + 1;
      } else {
        coords_x = coords_x + x_off[best_neighbour_i];
        coords_y = coords_y + y_off[best_neighbour_i];

        best_neighbour_i = 0;
        best_neighbour_stress = stress;

        printf("\n\nMoving to new tile: %d %d", coords_x, coords_y);
        progress = 0;
      }
    }
  }

  destroy();
  return EXIT_SUCCESS;
}
