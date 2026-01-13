#include <libpynq.h>
#include <time.h>
#include "../utils.h"
#define REFRESH_USEC 17500000

int main (void)
{
  init();
  
  printf("start");
  fflush(stdout);
  switchbox_set_pin(IO_AR0, SWB_UART0_RX);
  switchbox_set_pin(IO_AR1, SWB_UART0_TX);

  uart_init(UART0);
  uart_reset_fifos(UART0);
  
  uint32_t i;
  uint32_t slave_address1 = 0x70;
  uint32_t slave_address2 = 0x71;
  uint32_t slave_address3 = 0x72;
  
  int x_off[4] = {-1, 0, 0, 0};
  int y_off[4] = {0, -1, 0, 0};
  int coords_x = 4;
  int coords_y = 4;
  int progress = 0; // Neigbour checking progress

  int best_neighbour_stress = 101;
  int best_neighbour_i = 0;

  int cry = 100;
  int hb = 0;
  int hb_n = 0;
  int stress = 100;
  
  int skipped_neighbour = 0;

  __clock_t t = clock() - REFRESH_USEC;
  
  int ignore_x = 100;
  int ignore_y = 100;
  
  int path_x[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int path_y[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int path_i = 0;
  int follow_path = 0;
  int follow_path_i = 0;
  
  int evil_blocks_x[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int evil_blocks_y[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int evil_blocks_i = 0;

		uart_send (UART0, 0);
		uart_send (UART0, coords_x);
		uart_send (UART0, coords_y);
		
  while (1) {
    /*
      Crying
    */
    if (iic_read_register(IIC0, slave_address1, 0, (uint8_t *) &i, 4)) {
      printf("register[%d]=cry\n", 0);
      displayFillScreen(&display, RGB_BLACK);
      displayDrawString(&display, fx16G, 100, 100, (uint8_t *)"error", RGB_GREEN);
      break; 
    }
    if (i == 0) {
      uint8_t tt = 1;
      uint8_t *t = &tt;
      iic_write_register(IIC0, slave_address1, 0, t, 4);
      for (int reg=1; reg < 32; reg++) {
        iic_read_register(IIC0, slave_address1, reg, (uint8_t *) &i, 4);
        if (reg == 5){
          cry=i;
        }
      }
    }

    /*
      Heartbeat
    */
    if (clock() - t > REFRESH_USEC * 3 / 4 ) {
		int j = -1;
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
		      hb += j;
		      hb_n = hb_n + 1;
		      printf("hb %d\n", hb);
		    }
		  }
		}
    }

    uart_send (UART0, 1);
    
    if (clock() - t > REFRESH_USEC - 1000000) {
		uart_send (UART0, 0);
		uart_send (UART0, coords_x);
		uart_send (UART0, coords_y);
	}
    if (clock() - t > REFRESH_USEC) {
	    stress = hb / 2.2 / hb_n;
    		hb = 0;
    		hb_n = 0;
    		printf("Stress: %d \n\n", stress);
	    if (stress > 100) {
	   
	     stress = 100;
	     evil_blocks_x[evil_blocks_i] = coords_x + x_off[progress]; //CHECK IF PROGRESS AND NOT PROGRESS -1
 	     evil_blocks_y[evil_blocks_i] = coords_y + y_off[progress];
 	     evil_blocks_i += 1;
 	     if (path_i != 0) {
	     follow_path = 1;
	     follow_path_i = 0;
	     }
	     };
      t = clock();
      printf("Heartbeat %d, Crying: %d\n", hb, cry);

	if (follow_path) {
		printf("Following Path");
		uart_send (UART0, 0);
		uart_send (UART0, path_x[follow_path_i]);
		uart_send (UART0, path_y[follow_path_i]);
		coords_x = path_x[follow_path_i];
		coords_y = path_y[follow_path_i];
		follow_path_i += 1;
		if (follow_path_i >= path_i) {
			follow_path_i = 0;
			follow_path = 0;
		}
	}
	else {
		  if (progress < 5) {
		  	if (progress != 0 && !skipped_neighbour) {
				printf("Neighbour %d stress %d\n", progress-1, stress);
				if (stress < best_neighbour_stress) {
				  best_neighbour_stress = stress;
				  best_neighbour_i = progress-1;
				}
		    }
		    
			if(progress < 4) {
				while(1) {
					printf("Progress: %d\n", progress);
					int x1 = coords_x + x_off[progress];
					int y1 = coords_y + y_off[progress];
					
					int evil_block = 0;
					for (int p = 0; p < evil_blocks_i; p++) {
						if (evil_blocks_x[p] == x1 || evil_blocks_y[p] == y1) evil_block = 1;
					}
					
					if (x1 < 0 || x1 > 4 || y1 < 0 || y1 > 4 || x1 == ignore_x || y1 == ignore_y || evil_block) {
							progress = progress + 1;
							if (progress < 4) {
								skipped_neighbour = 1;
								continue;
							} else {
								skipped_neighbour = 1;
								break;
							}
					}
					int val = 0;

										printf("Testing %d %d\n\n", x1, y1);
					uart_send (UART0, 0);
					uart_send (UART0, x1);
					uart_send (UART0, y1);
					
					progress = progress + 1;
					skipped_neighbour = 0;
					break;
				}
		    }
		  }
		  if(progress>3) {
		  	ignore_x = coords_x;
		  	ignore_y = coords_y;
		  	path_x[path_i] = coords_x;
		  	path_y[path_i] = coords_y;
		  	path_i += 1;
		  	
		    coords_x = coords_x + x_off[best_neighbour_i];
		    coords_y = coords_y + y_off[best_neighbour_i];

			printf("Best neighbour %d.\n\n", best_neighbour_i);        
		    best_neighbour_i = 0;
		    stress = best_neighbour_stress;

			printf("Selected %d %d as the best candidate.\n\n", coords_x, coords_y);
			uart_send (UART0, 0);
			uart_send (UART0, coords_x);
			uart_send (UART0, coords_y);
		    progress = 0;
		  }
      }
    }
  }

  destroy();
  return EXIT_SUCCESS;
}
