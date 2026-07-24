#include <systemc>
#include <tlm>

#include "cli_parser_local.hh"
#include "report_handler.hh"
#include "accelerator.hh"
#include "sim_control.hh"
#include "slave_transactor.hh"
#include "master_transactor.hh"
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

    // Create gem5 - SystemC bridges
    Gem5SystemC::Gem5SlaveTransactor cfg_transactor("cfg_transactor",
                                                "cfg_transactor");
    
    Gem5SystemC::Gem5MasterTransactor dma_transactor("dma_transactor",
                                                    "dma_transactor");

    Accelerator accelerator("accelerator");

    // Bind sockets
    accelerator.cfg_socket.bind(cfg_transactor.socket);
    cfg_transactor.sim_control.bind(sim_control);

    accelerator.dma_socket.bind(dma_transactor.socket);
    dma_transactor.sim_control.bind(sim_control);

    SC_REPORT_INFO("sc_main", "Start of Simulation");

    sc_core::sc_start();

    SC_REPORT_INFO("sc_main", "End of Simulation");

    CxxConfig::statsDump();

    return EXIT_SUCCESS;
}
