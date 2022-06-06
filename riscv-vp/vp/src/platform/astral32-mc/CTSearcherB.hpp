#ifndef CTSEARCHER_B_H_
#define CTSEARCHER_B_H_

#include <systemc>
using namespace sc_core;

#include "Utility.h"

// Given 16 points upper hull r and q input, for their common tangent uv, where
// u belongs to r and v belongs to q, find block that contain u and the start
// index of A.
class CTSearcherB : public sc_module {
public:
    sc_fifo_in< sc_dt::sc_bigint<POINT*BLOCK_SIZE> > i_r_block;
    sc_fifo_in< sc_dt::sc_bigint<POINT*BLOCK_SIZE> > i_q_block;
    sc_fifo_in< sc_dt::sc_uint<INDEX_RANGE> > i_r_start_idx;
    sc_fifo_in< sc_dt::sc_uint<INDEX_RANGE> > i_q_start_idx;
    sc_fifo_out< sc_dt::sc_uint<INDEX_RANGE> > o_r_idx;
    sc_fifo_out< sc_dt::sc_uint<INDEX_RANGE> > o_q_idx;

    SC_HAS_PROCESS(CTSearcherB);
    CTSearcherB(sc_module_name n);
    ~CTSearcherB();
private:
    void searchCT();
    sc_dt::sc_bigint<SCALE> rx[BLOCK_SIZE];
    sc_dt::sc_bigint<SCALE> ry[BLOCK_SIZE];
    sc_dt::sc_bigint<SCALE> qx[BLOCK_SIZE];
    sc_dt::sc_bigint<SCALE> qy[BLOCK_SIZE];
};
#endif