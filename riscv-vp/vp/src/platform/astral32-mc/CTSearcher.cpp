#include "CTSearcher.hpp"
#include <iomanip>
#include <algorithm>

CTSearcher::CTSearcher( sc_module_name n): sc_module( n ), tsock("tsock"), base_offset(0),
	ctb_searcher_l("ctb_searcher_l"), ctb_searcher_r("ctb_searcher_r"), ct_searcher_b("ct_searcher_b")
{
    SC_THREAD(searchCT);
    tsock.register_b_transport(this, &CTSearcher::blocking_transport);

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
}

CTSearcher::~CTSearcher() {
}

void CTSearcher::searchCT() {
    while(true) {
        sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> _r = i_r.read();
        sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> _q = i_q.read();

        r_l.write(_r);
        r_r.write(_r);
        q_l.write(_q);
        q_r.write(_q);
        //wait(970*CLOCK_PERIOD, SC_NS); // simulate delay
        sc_dt::sc_uint<INDEX_RANGE> _r_idx = r_idx.read();
        sc_dt::sc_uint<INDEX_RANGE> _q_idx = q_idx.read();

        o_r_idx.write(_r_idx);
        o_q_idx.write(_q_idx);
    }
}

void CTSearcher::blocking_transport(tlm::tlm_generic_payload &payload,
                                    sc_core::sc_time &delay) {
    wait(delay);
    sc_dt::uint64 addr = payload.get_address();
    addr -= base_offset;
    unsigned char *data_ptr = payload.get_data_ptr();
    unsigned char buffer[4];
    sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> _r;
    sc_dt::sc_bigint<POINT*MAX_HULL_SIZE> _q;
    sc_dt::sc_uint<INDEX_RANGE> _r_idx;
    sc_dt::sc_uint<INDEX_RANGE> _q_idx;
    switch (payload.get_command()) {
    case tlm::TLM_READ_COMMAND:
        switch (addr) {
        case CTSEARCHER_RESULT_ADDR:
            _r_idx = o_r_idx.read();
            _q_idx = o_q_idx.read();
            buffer[0] = (unsigned char)_r_idx;
            buffer[1] = (unsigned char)_q_idx;
            buffer[2] = (unsigned char)0;
            buffer[3] = (unsigned char)0;
        break;
        default:
        std::cerr << "1Error! CTSearcher::blocking_transport: address 0x"
                    << std::setfill('0') << std::setw(8) << std::hex << addr
                    << std::dec << " is not valid" << std::endl;
                    exit(-1);
        }
        data_ptr[0] = buffer[0];
        data_ptr[1] = buffer[1];
        data_ptr[2] = buffer[2];
        data_ptr[3] = buffer[3];
        break;
    case tlm::TLM_WRITE_COMMAND:
        switch (addr) {
        case CTSEARCHER_R_ADDR:
            for (int i = 0; i < POINT*MAX_HULL_SIZE/8; ++i) {
                _r.range((i+1)*8-1, (i)*8) = data_ptr[i];
                _q.range((i+1)*8-1, (i)*8) = data_ptr[i+POINT*MAX_HULL_SIZE/8];
            }
            i_r.write(_r);
            i_q.write(_q);
        break;
        default:
            std::cerr << "2Error! CTSearcher::blocking_transport: address 0x"
                        << std::setfill('0') << std::setw(8) << std::hex << addr
                        << std::dec << " is not valid" << std::endl;
        }
        break;
    case tlm::TLM_IGNORE_COMMAND:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
    default:
        payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
        return;
    }
    payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}