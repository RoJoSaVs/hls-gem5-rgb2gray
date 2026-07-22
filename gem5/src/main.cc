

#include <systemc>
#include <tlm>

#include "cli_parser.hh"
#include "report_handler.hh"
#include "accelerator.hh"
#include "sim_control.hh"
#include "slave_transactor.hh"
#include "stats.hh"

int sc_main (int argc, char **argv)
{
    CliParser parser;
    parser.parse(argc, argv);

    sc_core::sc_report_handler::set_handler(reportHandler);

    Gem5SystemC::Gem5SimControl sim_control("gem5",
                                           parser.getConfigFile(),
                                           parser.getSimulationEnd(),
                                           parser.getDebugFlags());

    unsigned long long int accel_size = 256*1024*1024ULL;

    // TODO: connect transactor to accelerator
    Gem5SystemC::Gem5SlaveTransactor transactor("transactor", "transactor");
    
    memory.socket.bind(transactor.socket);
    transactor.sim_control.bind(sim_control);

    SC_REPORT_INFO("sc_main", "Start of Simulation");

    sc_core::sc_start();

    SC_REPORT_INFO("sc_main", "End of Simulation");

    CxxConfig::statsDump();

    return EXIT_SUCCESS;
}