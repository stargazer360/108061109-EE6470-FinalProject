#ifndef CTBSEARCHER_R_H_
#define CTBSEARCHER_R_H_
#include "Utility.h"

#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

// Given 16 points upper hull r and q input, for their common tangent uv, where
// u belongs to r and v belongs to q, find block that contain u and the start
// index of A.
class CTBSearcherR : public sc_module {
public:
    sc_in_clk i_clk;
    sc_in<bool> i_rst;
#ifndef NATIVE_SYSTEMC
    cynw_p2p< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> >::in i_r;
    cynw_p2p< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> >::in i_q;    
    cynw_p2p< sc_dt::sc_bigint<POINT*BLOCK_SIZE> >::out o_block;
    cynw_p2p< sc_dt::sc_uint<INDEX_RANGE> >::out o_start_idx;
#else
    sc_fifo_in< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > i_r;
    sc_fifo_in< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > i_q;    
    sc_fifo_out< sc_dt::sc_bigint<POINT*BLOCK_SIZE> > o_block;
    sc_fifo_out< sc_dt::sc_uint<INDEX_RANGE> > o_start_idx;
#endif
    SC_HAS_PROCESS(CTBSearcherR);
    CTBSearcherR(sc_module_name n);
    ~CTBSearcherR();
private:
    void searchCTB();
    sc_dt::sc_bigint<SCALE> rx[MAX_HULL_SIZE];
    sc_dt::sc_bigint<SCALE> ry[MAX_HULL_SIZE];
    sc_dt::sc_bigint<SCALE> qx[MAX_HULL_SIZE];
    sc_dt::sc_bigint<SCALE> qy[MAX_HULL_SIZE];
};
#endif