#include "System.h"
System::System( sc_module_name n, string in_file, string out_file): sc_module( n ), 
	tb("tb"), ctb_searcher_l("ctb_searcher_l"), ctb_searcher_r("ctb_searcher_r"), ct_searcher_b("ct_searcher_b"), 
    clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), out_file(out_file)
{
	tb.i_clk(clk);
	tb.o_rst(rst);
    ctb_searcher_l.i_clk(clk);
    ctb_searcher_l.i_rst(rst);
    ctb_searcher_r.i_clk(clk);
    ctb_searcher_r.i_rst(rst);
    ct_searcher_b.i_clk(clk);
    ct_searcher_b.i_rst(rst);

    tb.o_r_l(r_l);
    tb.o_q_l(q_l);
    tb.o_r_r(r_r);
    tb.o_q_r(q_r);

    ctb_searcher_l.i_r(r_l);
    ctb_searcher_l.i_q(q_l);
    ctb_searcher_r.i_r(r_r);
    ctb_searcher_r.i_q(q_r);

    ctb_searcher_l.o_block(r_block);
    ctb_searcher_l.o_start_idx(r_start_idx);
    ctb_searcher_r.o_block(q_block);
    ctb_searcher_r.o_start_idx(q_start_idx);

    ct_searcher_b.i_r_block(r_block);
    ct_searcher_b.i_q_block(q_block);
    ct_searcher_b.i_r_start_idx(r_start_idx);
    ct_searcher_b.i_q_start_idx(q_start_idx);
    
    ct_searcher_b.o_r_idx(r_idx);
    ct_searcher_b.o_q_idx(q_idx);
	tb.i_r_idx(r_idx);
	tb.i_q_idx(q_idx);

    tb.read_data(in_file);
}

System::~System() {

	tb.output_result(out_file);
}
