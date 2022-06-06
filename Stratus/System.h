#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Utility.h"
#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "CTBSearcherL_wrap.h"
#include "CTBSearcherR_wrap.h"
#include "CTSearcherB_wrap.h"
#else
#include "CTBSearcherL.h"
#include "CTBSearcherR.h"
#include "CTSearcherB.h"
#endif

class System: public sc_module
{
public:
	SC_HAS_PROCESS( System );
	System( sc_module_name n, std::string in_file, std::string out_file);
	~System();
private:
  	Testbench tb;
#ifndef NATIVE_SYSTEMC
    CTBSearcherL_wrapper ctb_searcher_l;
    CTBSearcherR_wrapper ctb_searcher_r;
    CTSearcherB_wrapper ct_searcher_b;
#else
    CTBSearcherL ctb_searcher_l;
    CTBSearcherR ctb_searcher_r;
    CTSearcherB ct_searcher_b;
#endif
	sc_clock clk;
	sc_signal<bool> rst;

#ifndef NATIVE_SYSTEMC
    cynw_p2p< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > r_l;
    cynw_p2p< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > q_l;
    cynw_p2p< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > r_r;
    cynw_p2p< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > q_r;  
    cynw_p2p< sc_dt::sc_bigint<POINT*BLOCK_SIZE> > r_block;
    cynw_p2p< sc_dt::sc_bigint<POINT*BLOCK_SIZE> > q_block;
    cynw_p2p< sc_dt::sc_uint<INDEX_RANGE> > r_start_idx;
    cynw_p2p< sc_dt::sc_uint<INDEX_RANGE> > q_start_idx;
    cynw_p2p< sc_dt::sc_uint<INDEX_RANGE> > r_idx;
    cynw_p2p< sc_dt::sc_uint<INDEX_RANGE> > q_idx;
#else
    sc_fifo< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > r_l;
    sc_fifo< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > q_l;
    sc_fifo< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > r_r;
    sc_fifo< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > q_r;  
    sc_fifo< sc_dt::sc_bigint<POINT*BLOCK_SIZE> > r_block;
    sc_fifo< sc_dt::sc_bigint<POINT*BLOCK_SIZE> > q_block;
    sc_fifo< sc_dt::sc_uint<INDEX_RANGE> > r_start_idx;
    sc_fifo< sc_dt::sc_uint<INDEX_RANGE> > q_start_idx;
    sc_fifo< sc_dt::sc_uint<INDEX_RANGE> > r_idx;
    sc_fifo< sc_dt::sc_uint<INDEX_RANGE> > q_idx;
#endif
    std::string out_file;
};
#endif
