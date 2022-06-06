#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <string>
using namespace std;
#include <systemc>
using namespace sc_core;

#include "Utility.h"

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

class Testbench : public sc_module {
public:
    sc_in_clk i_clk;
    sc_out<bool> o_rst;
#ifndef NATIVE_SYSTEMC
    cynw_p2p< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> >::base_out o_r_l;
    cynw_p2p< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> >::base_out o_q_l;
    cynw_p2p< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> >::base_out o_r_r;
    cynw_p2p< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> >::base_out o_q_r;  
    cynw_p2p< sc_dt::sc_uint<INDEX_RANGE> >::base_in i_r_idx;
    cynw_p2p< sc_dt::sc_uint<INDEX_RANGE> >::base_in i_q_idx;
#else
    sc_fifo_out< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > o_r_l;
    sc_fifo_out< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > o_q_l;
    sc_fifo_out< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > o_r_r;
    sc_fifo_out< sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> > o_q_r;  
    sc_fifo_in< sc_dt::sc_uint<INDEX_RANGE> > i_r_idx;
    sc_fifo_in< sc_dt::sc_uint<INDEX_RANGE> > i_q_idx;
#endif

    SC_HAS_PROCESS(Testbench);

    Testbench(sc_module_name n);
    ~Testbench();

    void read_data(string filename);
    void output_result(string filename);

private:
    int rx[MAX_HULL_SIZE];
    int ry[MAX_HULL_SIZE];
    int qx[MAX_HULL_SIZE];
    int qy[MAX_HULL_SIZE];
    unsigned int result_r_idx;
    unsigned int result_q_idx;
    void find_UCT(); // find upper common tangent
    void fetch_result();
};
#endif

