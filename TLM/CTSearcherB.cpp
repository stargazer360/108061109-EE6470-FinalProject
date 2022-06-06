#include "CTSearcherB.h"

CTSearcherB::CTSearcherB(sc_module_name n) : sc_module(n) {
    SC_THREAD(searchCT);
}

CTSearcherB::~CTSearcherB() {}

void CTSearcherB::searchCT() {
    while (true) {
        sc_dt::sc_uint<INDEX_RANGE> r_start_idx;
        sc_dt::sc_uint<INDEX_RANGE> q_start_idx;
        { // Input block
            sc_dt::sc_bigint<POINT*BLOCK_SIZE> r_block = i_r_block.read();
            sc_dt::sc_bigint<POINT*BLOCK_SIZE> q_block = i_q_block.read();
            r_start_idx = i_r_start_idx.read();
            q_start_idx = i_q_start_idx.read();
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                rx[i] = r_block.range(X_RANGE(i));
                ry[i] = r_block.range(Y_RANGE(i));
                qx[i] = q_block.range(X_RANGE(i));
                qy[i] = q_block.range(Y_RANGE(i));
            }
        }
        sc_dt::sc_uint<2> b_r_idx = BLOCK_SIZE - 1;
        sc_dt::sc_uint<2> b_q_idxs[BLOCK_SIZE];
        for (int i = 0; i < BLOCK_SIZE; ++i) {
            b_q_idxs[i] = BLOCK_SIZE - 1;
#ifdef KEY_3
            for (int j = 1; j <= KEY_SIZE; ++j) {
                sc_dt::sc_int<SCALE> diff_x = qx[j] - rx[i];
                sc_dt::sc_int<SCALE> diff_y = qy[j] - ry[i];
                b_q_idxs[i] -= (((sc_dt::sc_int<SCALE*2>)(diff_x * (qy[j-1] - ry[i]))) >= 
                                ((sc_dt::sc_int<SCALE*2>)(diff_y * (qx[j-1] - rx[i]))));
            }
            if (i != 0) {
                sc_dt::sc_uint<2> b_q_idx = b_q_idxs[i];
                sc_dt::sc_int<SCALE> diff_x = qx[b_q_idx] - rx[i];
                sc_dt::sc_int<SCALE> diff_y = qy[b_q_idx] - ry[i];
                b_r_idx -= (((sc_dt::sc_int<SCALE*2>)(diff_x * (qy[b_q_idx] - ry[i-1]))) <= 
                          ((sc_dt::sc_int<SCALE*2>)(diff_y * (qx[b_q_idx] - rx[i-1]))));
            }
#endif
        }
        sc_dt::sc_uint<4> q_idx = b_q_idxs[b_r_idx] + q_start_idx;
        sc_dt::sc_uint<4> r_idx = b_r_idx + r_start_idx;
        {
            o_r_idx.write(r_idx);
            o_q_idx.write(q_idx);
        }
    }
}