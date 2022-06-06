#ifndef UTILITY_H_
#define UTILITY_H_

#define MAX_HULL_SIZE 16
#define INDEX_RANGE 4
#define BLOCK_SIZE 4
#define SCALE 16
#define POINT SCALE*2
#define X_RANGE(i) SCALE*(i*2+1)-1, SCALE*(i*2)
#define Y_RANGE(i) SCALE*(i*2+2)-1, SCALE*(i*2+1)

const int CTSEARCHER_R_ADDR = 0x00000000;
const int CTSEARCHER_RESULT_ADDR = 0x00000004;
const int CTSEARCHER_CHECK_ADDR = 0x00000008;


#endif
