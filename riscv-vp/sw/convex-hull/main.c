#include "stdio.h"
#include "string.h"
#include "math.h"
#include "stdint.h"
#include "Utility.h"
#include "input.h"
#include "stdlib.h"
#include "acc.h"
#include "semaphore.h"

int main(unsigned hart_id) {
	uint32_t tangent_r_idx;
	uint32_t tangent_q_idx;
	unsigned char data[2*POINT*MAX_HULL_SIZE/8];

	uint32_t interval = 2;
	uint32_t r_hull_size;
	uint32_t q_hull_size;
	int16_t buffer[MAX_HULL_SIZE];

	if (hart_id == 0) {
		// create a barrier object with a count of PROCESSORS
		sem_init(&barrier_lock, 1);
		sem_init(&barrier_sem, 0); //lock all cores initially
		// Create mutex lock
		sem_init(&lock, 1);
	}

	if (hart_id < PROCESSORS) {
		for (int k = 0; k < DEPTH; k++) {
			for (int h_idx = hart_id*2*interval; h_idx < NUMBER_OF_POINTS; h_idx += 2*PROCESSORS*interval)
			{
				tangent_r_idx = 0;
				tangent_q_idx = 0;
				r_hull_size = length[h_idx];
				q_hull_size = length[h_idx+interval];
				for (int i = 0; i < MAX_HULL_SIZE; ++i) {
					if (i < r_hull_size) {
						data[4*i+0] =  input_x[h_idx+i] & 0x00ff;
						data[4*i+1] = (input_x[h_idx+i] & 0xff00) >> 8;
						data[4*i+2] =  input_y[h_idx+i] & 0x00ff;
						data[4*i+3] = (input_y[h_idx+i] & 0xff00) >> 8;
					} else {
						data[4*i+0] =  input_x[h_idx+r_hull_size-1] & 0x00ff;
						data[4*i+1] = (input_x[h_idx+r_hull_size-1] & 0xff00) >> 8;
						data[4*i+2] = 0x00;
						data[4*i+3] = 0x00;
					}
					if (i < q_hull_size) {
						data[4*(i+MAX_HULL_SIZE)+0] =  input_x[h_idx+interval+i] & 0x00ff;
						data[4*(i+MAX_HULL_SIZE)+1] = (input_x[h_idx+interval+i] & 0xff00) >> 8;
						data[4*(i+MAX_HULL_SIZE)+2] =  input_y[h_idx+interval+i] & 0x00ff;
						data[4*(i+MAX_HULL_SIZE)+3] = (input_y[h_idx+interval+i] & 0xff00) >> 8;
					} else {
						data[4*(i+MAX_HULL_SIZE)+0] =  input_x[h_idx+interval+q_hull_size-1] & 0x00ff;
						data[4*(i+MAX_HULL_SIZE)+1] = (input_x[h_idx+interval+q_hull_size-1] & 0xff00) >> 8;
						data[4*(i+MAX_HULL_SIZE)+2] = 0x00;
						data[4*(i+MAX_HULL_SIZE)+3] = 0x00;
					}
				}
				sem_wait(&lock);
				write_data_to_ACC(CTSEARCHER_START_ADDR + hart_id*0x01000000, data, 2*POINT*MAX_HULL_SIZE/8);
				sem_post(&lock);
				sem_wait(&lock);
				read_data_from_ACC(CTSEARCHER_READ_ADDR + hart_id*0x01000000 , data, 2);
				sem_post(&lock);

				tangent_r_idx = data[0];
				tangent_q_idx = data[1];
				// printf("tangent r:, %u, tangent q: %u\n", tangent_r_idx, tangent_q_idx);
				length[h_idx] =  tangent_r_idx + 1 + length[h_idx+interval] - tangent_q_idx;
				// printf("length: %u, h_idx: %d\n", length[h_idx], h_idx);;
				memcpy(buffer, (input_x+h_idx+interval+tangent_q_idx), (length[h_idx+interval] - tangent_q_idx) * sizeof(int16_t));
				memcpy((input_x+h_idx+tangent_r_idx+1), buffer, (length[h_idx+interval] - tangent_q_idx) * sizeof(int16_t));
				memcpy(buffer, (input_y+h_idx+interval+tangent_q_idx), (length[h_idx+interval] - tangent_q_idx) * sizeof(int16_t));
				memcpy((input_y+h_idx+tangent_r_idx+1), buffer, (length[h_idx+interval] - tangent_q_idx) * sizeof(int16_t));
			}
			interval*=2;
			barrier(&barrier_sem, &barrier_lock, &barrier_counter, PROCESSORS);
		}
		
		if (hart_id == 0) {
			for (int i = 0; i < NUMBER_OF_POINTS/4; i++) {
				printf("%d, ", input_x[i]);
			}
			printf("\n");
			for (int i = 0; i < NUMBER_OF_POINTS/4; i++) {
				printf("%d, ", input_x[i+NUMBER_OF_POINTS/4]);
			}
			printf("\n");
			for (int i = 0; i < NUMBER_OF_POINTS/4; i++) {
				printf("%d, ", input_x[i+2*NUMBER_OF_POINTS/4]);
			}
			printf("\n");
			for (int i = 0; i < NUMBER_OF_POINTS/4; i++) {
				printf("%d, ", input_x[i+3*NUMBER_OF_POINTS/4]);
			}
			printf("\n");
			printf("\n");

			for (int i = 0; i < NUMBER_OF_POINTS/4; i++) {
				printf("%d, ", input_y[i]);
			}
			printf("\n");
			for (int i = 0; i < NUMBER_OF_POINTS/4; i++) {
				printf("%d, ", input_y[i+NUMBER_OF_POINTS/4]);
			}
			printf("\n");
			for (int i = 0; i < NUMBER_OF_POINTS/4; i++) {
				printf("%d, ", input_y[i+2*NUMBER_OF_POINTS/4]);
			}
			printf("\n");
			for (int i = 0; i < NUMBER_OF_POINTS/4; i++) {
				printf("%d, ", input_y[i+3*NUMBER_OF_POINTS/4]);
			}
			printf("\n");
		}
	}

}
