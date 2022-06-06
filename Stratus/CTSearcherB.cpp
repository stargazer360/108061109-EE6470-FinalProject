#include "CTSearcherB.h"

#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

CTSearcherB::CTSearcherB(sc_module_name n) : sc_module(n) {
    SC_THREAD(searchCT);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
#ifndef NATIVE_SYSTEMC
    i_r_block.clk_rst(i_clk, i_rst);
    i_q_block.clk_rst(i_clk, i_rst);
    i_r_start_idx.clk_rst(i_clk, i_rst);
    i_q_start_idx.clk_rst(i_clk, i_rst);
    o_r_idx.clk_rst(i_clk, i_rst);
    o_q_idx.clk_rst(i_clk, i_rst);
    HLS_FLATTEN_ARRAY(rx);
    HLS_FLATTEN_ARRAY(ry);
    HLS_FLATTEN_ARRAY(qx);
    HLS_FLATTEN_ARRAY(qy);
#endif
}

CTSearcherB::~CTSearcherB() {}

void CTSearcherB::searchCT() {
    { // Reset block
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        i_r_block.reset();
        i_q_block.reset();
        i_r_start_idx.reset();
        i_q_start_idx.reset();
        o_r_idx.reset();
        o_q_idx.reset();
#endif
        wait();
    }

    while (true) {
        HLS_PIPELINE_LOOP(HARD_STALL, 2, "main_pipeline");
        sc_dt::sc_uint<INDEX_RANGE> r_start_idx;
        sc_dt::sc_uint<INDEX_RANGE> q_start_idx;
        { // Input block
#ifndef NATIVE_SYSTEMC
            //HLS_DEFINE_PROTOCOL("input");
            sc_dt::sc_bigint<POINT*BLOCK_SIZE> r_block = i_r_block.get();
            sc_dt::sc_bigint<POINT*BLOCK_SIZE> q_block = i_q_block.get();
            r_start_idx = i_r_start_idx.get();
            q_start_idx = i_q_start_idx.get();
#else
            sc_dt::sc_bigint<POINT*BLOCK_SIZE> r_block = i_r_block.read();
            sc_dt::sc_bigint<POINT*BLOCK_SIZE> q_block = i_q_block.read();
            r_start_idx = i_r_start_idx.read();
            q_start_idx = i_q_start_idx.read();
#endif
            for (int i = 0; i < BLOCK_SIZE; ++i) {
                rx[i] = r_block.range(X_RANGE(i));
                ry[i] = r_block.range(Y_RANGE(i));
                qx[i] = q_block.range(X_RANGE(i));
                qy[i] = q_block.range(Y_RANGE(i));
            }
        }
        sc_dt::sc_uint<2> b_r_idx = BLOCK_SIZE - 1;
        sc_dt::sc_uint<2> b_q_idxs[BLOCK_SIZE];
        HLS_FLATTEN_ARRAY(b_q_idxs);
        for (int i = 0; i < BLOCK_SIZE; ++i) {
#ifndef NATIVE_SYSTEMC
            //HLS_PIPELINE_LOOP(HARD_STALL, 1, "pipeline");
            HLS_UNROLL_LOOP(ON, "main loop");
#endif
            b_q_idxs[i] = BLOCK_SIZE - 1;
#ifdef KEY_3
            for (int j = 1; j <= KEY_SIZE; ++j) {
#ifndef NATIVE_SYSTEMC
                HLS_UNROLL_LOOP(ON, "find q loop");
                HLS_CONSTRAIN_LATENCY(0, 50, "find_q");
#endif
                sc_dt::sc_int<SCALE> diff_x = qx[j] - rx[i];
                sc_dt::sc_int<SCALE> diff_y = qy[j] - ry[i];
                b_q_idxs[i] -= (((sc_dt::sc_int<SCALE*2>)(diff_x * (qy[j-1] - ry[i]))) >= 
                                ((sc_dt::sc_int<SCALE*2>)(diff_y * (qx[j-1] - rx[i]))));
            }
            if (i != 0) {
#ifndef NATIVE_SYSTEMC
                HLS_CONSTRAIN_LATENCY(0, 50, "check_r");
#endif
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
#ifndef NATIVE_SYSTEMC
            //HLS_DEFINE_PROTOCOL("output");
            o_r_idx.put(r_idx);
            o_q_idx.put(q_idx);
#else
            o_r_idx.write(r_idx);
            o_q_idx.write(q_idx);
#endif
        }
    }
}