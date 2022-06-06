#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <set>
#include <cmath>

#include "Testbench.h"

Testbench::Testbench(sc_module_name n) : sc_module(n), initiator("initiator") {
    SC_THREAD(find_UCT);
}

Testbench::~Testbench(){}

void Testbench::find_UCT() {
    sc_time last_out = sc_time_stamp();

    unsigned char data[2*POINT*MAX_HULL_SIZE/8];
    unsigned char mask[2*POINT*MAX_HULL_SIZE/8];
    for (int k = 0; k < 5; k++) {
        sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> r;
        sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> q;
        for (int i = 0; i < MAX_HULL_SIZE; ++i) {
            r.range(X_RANGE(i)) = rx[i];
            r.range(Y_RANGE(i)) = ry[i];
            q.range(X_RANGE(i)) = qx[i];
            q.range(Y_RANGE(i)) = qy[i];
        }
        sc_dt::sc_int<8> tmp;
        for (int i = 0; i < POINT*MAX_HULL_SIZE/8; ++i) {
            tmp = r.range((i+1)*8-1, (i)*8);        
            data[i] = (unsigned char)tmp;
            mask[i] = 0xff;

            tmp = q.range((i+1)*8-1, (i)*8);
            data[i+POINT*MAX_HULL_SIZE/8] = (unsigned char)tmp;
            mask[i+POINT*MAX_HULL_SIZE/8] = 0xff;
        }

        initiator.write_to_socket(CTSEARCHER_MM_BASE + CTSEARCHER_R_ADDR, mask, data, 2*POINT*MAX_HULL_SIZE/8);

        int output_num = 0;
        while (output_num<=0) {
            initiator.read_from_socket(CTSEARCHER_MM_BASE + CTSEARCHER_CHECK_ADDR, mask, data, 4);
            unsigned int tmp = 0;
            tmp |= (uint32_t)data[0];
            tmp |= (uint32_t)data[1] << 8;
            tmp |= (uint32_t)data[2] << 16;
            tmp |= (uint32_t)data[3] << 24;
            output_num = (int)tmp;
        }
        wait(970 * CLOCK_PERIOD, SC_NS);
        initiator.read_from_socket(CTSEARCHER_MM_BASE + CTSEARCHER_RESULT_ADDR, mask, data, 2);
        result_r_idx = data[0];
        result_q_idx = data[1];

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