#include "CTBSearcherL.hpp"

CTBSearcherL::CTBSearcherL(sc_module_name n) : sc_module(n) {
    SC_THREAD(searchCTB);
}

CTBSearcherL::~CTBSearcherL() {}

void CTBSearcherL::searchCTB() {
    while (true) {
        { // Input block
            sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> r = i_r.read();
            sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> q = i_q.read();
            for (int i = 0; i < MAX_HULL_SIZE; ++i) {
                rx[i] = r.range(X_RANGE(i));
                ry[i] = r.range(Y_RANGE(i));
                qx[i] = q.range(X_RANGE(i));
                qy[i] = q.range(Y_RANGE(i));
            }
        }
        sc_dt::sc_uint<2> block_idx = 3;
        for (int i = 1; i < (MAX_HULL_SIZE/BLOCK_SIZE); ++i) {
#ifdef KEY_3
            sc_dt::sc_uint<2> d1_idx = 3;
            for (int j = 1; j <= KEY_SIZE; ++j) { // depth 1
                sc_dt::sc_int<SCALE> diff_x = qx[j*DEPTH_1_GAP] - rx[i*BLOCK_SIZE];
                sc_dt::sc_int<SCALE> diff_y = qy[j*DEPTH_1_GAP] - ry[i*BLOCK_SIZE];
                d1_idx -= (((sc_dt::sc_int<SCALE*2>)(diff_x * (qy[j*DEPTH_1_GAP-1] - ry[i*BLOCK_SIZE]))) >= 
                          ((sc_dt::sc_int<SCALE*2>)(diff_y * (qx[j*DEPTH_1_GAP-1] - rx[i*BLOCK_SIZE]))));
            }
            sc_dt::sc_uint<2> d2_idx = 3;
            for (int j = 1; j <= KEY_SIZE; ++j) { // depth 2
                sc_dt::sc_int<SCALE> diff_x = qx[d1_idx*DEPTH_1_GAP + j*DEPTH_2_GAP] - rx[i*BLOCK_SIZE];
                sc_dt::sc_int<SCALE> diff_y = qy[d1_idx*DEPTH_1_GAP + j*DEPTH_2_GAP] - ry[i*BLOCK_SIZE];
                d2_idx -= (((sc_dt::sc_int<SCALE*2>)(diff_x * (qy[d1_idx*DEPTH_1_GAP + j*DEPTH_2_GAP-1] - ry[i*BLOCK_SIZE]))) >= 
                           ((sc_dt::sc_int<SCALE*2>)(diff_y * (qx[d1_idx*DEPTH_1_GAP + j*DEPTH_2_GAP-1] - rx[i*BLOCK_SIZE]))));
            }

            {   // find block
                sc_dt::sc_uint<4> q_idx = d1_idx*DEPTH_1_GAP + d2_idx*DEPTH_2_GAP;
                sc_dt::sc_int<SCALE> diff_x = qx[q_idx] - rx[i*BLOCK_SIZE];
                sc_dt::sc_int<SCALE> diff_y = qy[q_idx] - ry[i*BLOCK_SIZE];
                block_idx -= (((sc_dt::sc_int<SCALE*2>)(diff_x * (qy[q_idx] - ry[i*BLOCK_SIZE-1]))) <= 
                              ((sc_dt::sc_int<SCALE*2>)(diff_y * (qx[q_idx] - rx[i*BLOCK_SIZE-1]))));
            }
#endif
        }
        sc_dt::sc_uint<INDEX_RANGE> start_idx = block_idx*BLOCK_SIZE;
        sc_dt::sc_bigint<POINT*BLOCK_SIZE> block;
        for (int i = 0; i < BLOCK_SIZE; ++i) {
            block.range(X_RANGE(i)) = rx[start_idx+i];
            block.range(Y_RANGE(i)) = ry[start_idx+i];
        }
        
        {
            o_block.write(block);
            o_start_idx.write(start_idx);
        }
    }
}