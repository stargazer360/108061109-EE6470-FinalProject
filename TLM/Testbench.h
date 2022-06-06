#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <string>
using namespace std;
#include <systemc>
using namespace sc_core;

#include "Utility.h"


#include "Initiator.h"
#include "MemoryMap.h"

class Testbench : public sc_module {
public:
    Initiator initiator;

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
};
#endif

