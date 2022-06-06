#ifndef CTSEARCHER_H_
#define CTSEARCHER_H_

#include <systemc>
using namespace sc_core;

#include <tlm>
#include <tlm_utils/simple_target_socket.h>

#include "CTBSearcherL.hpp"
#include "CTBSearcherR.hpp"
#include "CTSearcherB.hpp"

#include "Utility.h"

class CTSearcher: public sc_module
{
public:
    tlm_utils::simple_target_socket<CTSearcher> tsock;


	SC_HAS_PROCESS(CTSearcher);
	CTSearcher(sc_module_name n);
	~CTSearcher();
private:
    CTBSearcherL ctb_searcher_l;
    CTBSearcherR ctb_searcher_r;
    CTSearcherB ct_searcher_b;

    sc_fifo< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > i_r;
    sc_fifo< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > i_q;
    sc_fifo< sc_dt::sc_uint<INDEX_RANGE> > o_r_idx;
    sc_fifo< sc_dt::sc_uint<INDEX_RANGE> > o_q_idx;


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

private: 
    void searchCT();

    unsigned int base_offset;
    void blocking_transport(tlm::tlm_generic_payload &payload,
                            sc_core::sc_time &delay);
};
#endif
