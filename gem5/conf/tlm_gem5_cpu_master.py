import m5
from m5.objects import *


system = System()

system.clk_domain = SrcClockDomain(
    clock="250MHz",
    voltage_domain=VoltageDomain("1V")
)

system.cpu = ArmTimingSimpleCPU()

# Bus
system.membus = IOXBar(width=16)

# RAM memory
system.mem_ctrl = SimpleMemory()

# TODO: Check if init addr needs to be specified
system.mem_ctrl.range = AddrRange(size="64MiB")

# Bridge to SystemC module
system.tlm = ExternalSlave()
# TODO: Update address ranges
system.tlm.addr_ranges = [AddrRange("64MiB")]
system.tlm.port_type = "tlm_slave"
system.tlm.port_data = "transactor"

# TODO: Set binary example to load on CPU

# Connections
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.icache_port = system.membus.cpu_side_ports

system.mem_ctrl.port = system.membus.mem_side_ports

system.membus.mem_side_ports = system.tlm.port

# Simulation start
root = Root(full_system=False, system=system)
root.system.mem_mode = "timing"
m5.instantiate()

# TODO: Set simulation time or allow for cmdline setting
m5.simulate()