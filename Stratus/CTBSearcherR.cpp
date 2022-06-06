#include "CTBSearcherR.h"

#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif

CTBSearcherR::CTBSearcherR(sc_module_name n) : sc_module(n) {
    SC_THREAD(searchCTB);
    sensitive << i_clk.pos();
    dont_initialize();
    reset_signal_is(i_rst, false);
#ifndef NATIVE_SYSTEMC
    i_r.clk_rst(i_clk, i_rst);
    i_q.clk_rst(i_clk, i_rst);
    o_block.clk_rst(i_clk, i_rst);
    o_start_idx.clk_rst(i_clk, i_rst);
    HLS_FLATTEN_ARRAY(rx);
    HLS_FLATTEN_ARRAY(ry);
    HLS_FLATTEN_ARRAY(qx);
    HLS_FLATTEN_ARRAY(qy);
#endif
}

CTBSearcherR::~CTBSearcherR() {}

void CTBSearcherR::searchCTB() {
    { // Reset block
#ifndef NATIVE_SYSTEMC
        HLS_DEFINE_PROTOCOL("main_reset");
        i_r.reset();
        i_q.reset();
        o_block.reset();
        o_start_idx.reset();
#endif
        wait();
    }

    while (true) {
        HLS_PIPELINE_LOOP(HARD_STALL, 2, "main_pipeline");
        { // Input block
#ifndef NATIVE_SYSTEMC
            //HLS_DEFINE_PROTOCOL("input");
            sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> r = i_r.get();
            sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> q = i_q.get();
#else
            sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> r = i_r.read();
            sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> q = i_q.read();
#endif
            for (int i = 0; i < MAX_HULL_SIZE; ++i) {
                rx[i] = r.range(X_RANGE(i));
                ry[i] = r.range(Y_RANGE(i));
                qx[i] = q.range(X_RANGE(i));
                qy[i] = q.range(Y_RANGE(i));
            }
        }
        sc_dt::sc_uint<2> block_idx = 3;
        for (int i = 1; i < (MAX_HULL_SIZE/BLOCK_SIZE); ++i) {
            //HLS_PIPELINE_LOOP(HARD_STALL, 1, "pipeline");
            HLS_UNROLL_LOOP(ON, "main loop");
#ifndef NATIVE_SYSTEMC
#endif
#ifdef KEY_3
            sc_dt::sc_uint<2> d1_idx = 3;
            for (int j = 1; j <= KEY_SIZE; ++j) { // depth 1
#ifndef NATIVE_SYSTEMC
                HLS_UNROLL_LOOP(ON, "depth 1 loop");
                HLS_CONSTRAIN_LATENCY(0, 50, "r_depth_1");
#endif
                sc_dt::sc_int<SCALE> diff_x = qx[i*BLOCK_SIZE] - rx[j*DEPTH_1_GAP];
                sc_dt::sc_int<SCALE> diff_y = qy[i*BLOCK_SIZE] - ry[j*DEPTH_1_GAP];
                d1_idx -= (((sc_dt::sc_int<SCALE*2>)(diff_x * (qy[i*BLOCK_SIZE] - ry[j*DEPTH_1_GAP-1]))) <= 
                           ((sc_dt::sc_int<SCALE*2>)(diff_y * (qx[i*BLOCK_SIZE] - rx[j*DEPTH_1_GAP-1]))));
            }

            sc_dt::sc_uint<2> d2_idx = 3;
            for (int j = 1; j <= KEY_SIZE; ++j) { // depth 2
#ifndef NATIVE_SYSTEMC
                HLS_UNROLL_LOOP(ON, "depth 2 loop");
                HLS_CONSTRAIN_LATENCY(0, 50, "r_depth_2");
#endif
                sc_dt::sc_int<SCALE> diff_x = qx[i*BLOCK_SIZE] - rx[d1_idx*DEPTH_1_GAP + j*DEPTH_2_GAP];
                sc_dt::sc_int<SCALE> diff_y = qy[i*BLOCK_SIZE] - ry[d1_idx*DEPTH_1_GAP + j*DEPTH_2_GAP];
                d2_idx -= (((sc_dt::sc_int<SCALE*2>)(diff_x * (qy[i*BLOCK_SIZE] - ry[d1_idx*DEPTH_1_GAP + j*DEPTH_2_GAP - 1]))) <= 
                           ((sc_dt::sc_int<SCALE*2>)(diff_y * (qx[i*BLOCK_SIZE] - rx[d1_idx*DEPTH_1_GAP + j*DEPTH_2_GAP - 1]))));
            }
            {   // find block
#ifndef NATIVE_SYSTEMC
                HLS_CONSTRAIN_LATENCY(0, 50, "r_find_block");
#endif
                sc_dt::sc_uint<4> r_idx = d1_idx*DEPTH_1_GAP + d2_idx*DEPTH_2_GAP;

                sc_dt::sc_int<SCALE> diff_x = qx[i*BLOCK_SIZE] - rx[r_idx];
                sc_dt::sc_int<SCALE> diff_y = qy[i*BLOCK_SIZE] - ry[r_idx];
                block_idx -= (((sc_dt::sc_int<SCALE*2>)(diff_x * (qy[i*BLOCK_SIZE-1] - ry[r_idx]))) >= 
                              ((sc_dt::sc_int<SCALE*2>)(diff_y * (qx[i*BLOCK_SIZE-1] - rx[r_idx]))));
            }
#endif
        }
        sc_dt::sc_uint<INDEX_RANGE> start_idx = block_idx*BLOCK_SIZE;
        sc_dt::sc_bigint<POINT*BLOCK_SIZE> block;
        for (int i = 0; i < BLOCK_SIZE; ++i) {
            block.range(X_RANGE(i)) = qx[start_idx+i];
            block.range(Y_RANGE(i)) = qy[start_idx+i];
        }
        
        {
#ifndef NATIVE_SYSTEMC
            //HLS_DEFINE_PROTOCOL("output");
            o_block.put(block);
            o_start_idx.put(start_idx);
#else
            o_block.write(block);
            o_start_idx.write(start_idx);
#endif
        }
    }
}