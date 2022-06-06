#include <iostream>
#include <string>
using namespace std;


// Wall Clock Time Measurement
#include <sys/time.h>

#include "SimpleBus.h"
#include "CTSearcher.h"
#include "Testbench.h"

struct timeval start_time, end_time;

int sc_main(int argc, char **argv) {
    if ((argc < 3) || (argc > 4)) {
        cout << "No arguments for the executable : " << argv[0] << endl;
        cout << "Usage : >" << argv[0] << "data_file_name result_file_name"
         << endl;
        return 0;
    }
    Testbench tb("tb");
    SimpleBus<1, 1> bus("bus");
    bus.set_clock_period(sc_time(CLOCK_PERIOD, SC_NS));
    CTSearcher ct_searcher("ct_searcher");
    tb.initiator.i_skt(bus.t_skt[0]);
    bus.setDecode(0, CTSEARCHER_MM_BASE, CTSEARCHER_MM_BASE + CTSEARCHER_MM_SIZE - 1);
    bus.i_skt[0](ct_searcher.t_skt);
    tb.read_data(argv[1]);
    sc_start();
    std::cout << "Simulated time == " << sc_core::sc_time_stamp() << std::endl;
    tb.output_result(argv[2]);
    return 0;
}
