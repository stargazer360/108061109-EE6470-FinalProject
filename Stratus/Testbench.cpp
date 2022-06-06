#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <set>
#include <cmath>

#include "Testbench.h"

Testbench::Testbench(sc_module_name n) : sc_module(n) {
    SC_THREAD(find_UCT);
    sensitive << i_clk.pos();
    dont_initialize();
    SC_THREAD(fetch_result);
    sensitive << i_clk.pos();
    dont_initialize();
}

Testbench::~Testbench(){}

void Testbench::find_UCT() {
#ifndef NATIVE_SYSTEMC
    o_r_l.reset();
    o_q_l.reset();
    o_r_r.reset();
    o_q_r.reset();  
#endif
    o_rst.write(false);
    wait(5);
    o_rst.write(true);
    wait(1);
    for (int k = 0; k < 5; k++) {
        sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> r_l;
        sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> q_l;
        sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> r_r;
        sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> q_r;
        for (int i = 0; i < MAX_HULL_SIZE; ++i) {
            r_l.range(X_RANGE(i)) = rx[i];
            r_l.range(Y_RANGE(i)) = ry[i];

            q_l.range(X_RANGE(i)) = qx[i];
            q_l.range(Y_RANGE(i)) = qy[i];

            r_r.range(X_RANGE(i)) = rx[i];
            r_r.range(Y_RANGE(i)) = ry[i];

            q_r.range(X_RANGE(i)) = qx[i];
            q_r.range(Y_RANGE(i)) = qy[i];
        }
    #ifndef NATIVE_SYSTEMC
        o_r_l.put(r_l);
        o_q_l.put(q_l);
        o_r_r.put(r_r);
        o_q_r.put(q_r);  
    #else
        o_r_l.write(r_l);
        o_q_l.write(q_l);
        o_r_r.write(r_r);
        o_q_r.write(q_r);
    #endif
    }
}

void Testbench::fetch_result() {
#ifndef NATIVE_SYSTEMC
    i_r_idx.reset();
    i_q_idx.reset();
#endif
    wait(5);
    wait(1);
    sc_time last_out = sc_time_stamp();

    //std::cout << sc_time_stamp() - last_out << std::endl;
    for (int k = 0; k < 5; k++) {
#ifndef NATIVE_SYSTEMC
        result_r_idx = i_r_idx.get();
        result_q_idx = i_q_idx.get();
#else
        result_r_idx = i_r_idx.read();
        result_q_idx = i_q_idx.read();
#endif
        std::cout << sc_time_stamp()-last_out << std::endl;
        std::cout << result_r_idx << " " << result_q_idx  << std::endl;
        last_out = sc_time_stamp();
    }
    sc_stop();
}

void Testbench::read_data(string filename) {
    std::fstream in_file;
    in_file.open(filename.c_str(), std::fstream::binary | std::fstream::in);
    if (in_file.is_open()) {
        for (int i = 0; i < MAX_HULL_SIZE; ++i) {
            in_file >> rx[i] >> ry[i];
        }
        for (int i = 0; i < MAX_HULL_SIZE; ++i) {
            in_file >> qx[i] >> qy[i];
        }
        in_file.close();
    } else {
        std::cout << "Fail to open: " << filename;
        exit(-1);
    }
}

void Testbench::output_result(string filename) {
    std::fstream out_file;
    std::cout << filename << std::endl;
    out_file.open(filename.c_str(), std::fstream::binary | std::fstream::out);
    if (out_file.is_open()) {
        out_file << result_r_idx << " " << result_q_idx << "\n";
        out_file << rx[result_r_idx]  << " " << ry[result_r_idx] << "\n";
        out_file << qx[result_q_idx]  << " " << qy[result_q_idx] << "\n";
        out_file.close();
    } else {
        std::cout << "Fail to open: " << filename;
    }
}