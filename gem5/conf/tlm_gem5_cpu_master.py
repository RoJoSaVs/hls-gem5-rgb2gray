import argparse
import os
import m5
from m5.objects import *

parser = argparse.ArgumentParser()
parser.add_argument(
    "--image",
    required=True,
    help="Raw RGB image to convert,
    default="#../../pictures/raw/grumpy-online.raw"
)
args = parser.parse_args()
image_path = os.path.abspath(args.image)

system = System()

system.clk_domain = SrcClockDomain(
    clock="250MHz",
    voltage_domain=VoltageDomain("1V")
)

system.cpu = ArmTimingSimpleCPU()

# Bus
system.membus = IOXBar(width=16)

# RAM memory
system.ram = SimpleMemory()

system.ram.range = AddrRange(
    start=0x00000000,
    size="64MiB"
)

# 'Persistent' memory
raw_image = os.path.join(image_path)

system.nvm = SimpleMemory()

system.nvm.range = AddrRange(
    start=0x20000000,
    size="1000MiB"
)

# Set the image into the memory
system.nvm.image_file = raw_image

system.nvm.bandwidth = "4GiB/s"

# Bridge to SystemC module
system.tlm = ExternalSlave()
# Memory start according to the initial memory map
system.tlm.addr_ranges = [
    AddrRange(
        start=0x10000000,
        size="256MiB"
    )
]

system.nvm.latency = "1us"
system.nvm.bandwidth = "500MiB/s"

system.tlm.port_type = "tlm_slave"
system.tlm.port_data = "transactor"

# TODO: Set binary example to load on CPU

# Connections
system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.cpu_side_ports

system.ram.port = system.membus.mem_side_ports

system.nvm.port = system.membus.mem_side_ports

system.membus.mem_side_ports = system.tlm.port

# Simulation start
root = Root(full_system=False, system=system)
root.system.mem_mode = "timing"
m5.instantiate()

# TODO: Set simulation time or allow for cmdline setting
m5.simulate()