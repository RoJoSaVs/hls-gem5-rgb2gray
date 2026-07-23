import argparse
import os
import m5
from m5.objects import *

parser = argparse.ArgumentParser()
parser.add_argument(
    "--image",
    help="Raw RGB image to convert",
    default="grumpy-online.raw"
)

parser.add_argument(
    "--binary",
    help="AArch64 bare-metal ELF workload",
    default="rgb2gray-arm.elf"
)

args = parser.parse_args()
image_path = os.path.abspath(args.image)
binary_path = os.path.abspath(args.binary)

system = ArmSystem()
system.highest_el_is_64 = True
system.auto_reset_addr = True

system.clk_domain = SrcClockDomain(
    clock="250MHz",
    voltage_domain=VoltageDomain(voltage="1V")
)

# CPU
system.cpu = ArmTimingSimpleCPU()

system.cpu.createInterruptController()

# Bus
system.membus = IOXBar(width=16)

# RAM memory
system.ram = SimpleMemory()

system.ram.range = AddrRange(
    start=0x00000000,
    size="64MiB"
)

system.ram.latency = "1ns"
system.ram.latency_var = "0ns"
system.ram.bandwidth = "4GiB/s"

# 'Persistent' memory
raw_image = os.path.join(image_path)

system.nvm = SimpleMemory()

system.nvm.range = AddrRange(
    start=0x20000000,
    size="1000MiB"
)

# Set the image into the memory
system.nvm.image_file = raw_image

system.nvm.latency = "10ns"
system.nvm.latency_var = "0ns"
system.nvm.bandwidth = "500MiB/s"

# Bridge from gem5 cpu to SystemC module
system.accel_cfg = ExternalSlave()
# Memory start according to the initial memory map
system.accel_cfg.addr_ranges = [
    AddrRange(
        start=0x10000000,
        size=256
    )
]

system.accel_cfg.port_type = "tlm_slave"
system.accel_cfg.port_data = "cfg_transactor"

# Bridge from SystemC module to gem5 modules
system.accel_dma = ExternalMaster()
system.accel_dma.port_type = "tlm_master"
system.accel_dma.port_data = "dma_transactor"

system.mem_ranges = [
    system.ram.range,
    system.nvm.range,
]

# Load C program to the CPU
system.workload = ArmFsWorkload(
    object_file=binary_path,
)

system.workload.addr_check = False

system.cpu.createThreads()

# Connections
system.system_port = system.membus.cpu_side_ports

system.cpu.icache_port = system.membus.cpu_side_ports
system.cpu.dcache_port = system.membus.cpu_side_ports

system.ram.port = system.membus.mem_side_ports

system.nvm.port = system.membus.mem_side_ports

system.membus.mem_side_ports = system.accel_cfg.port

system.accel_dma.port = system.membus.cpu_side_ports

# Simulation start
root = Root(full_system=True, system=system)
root.system.mem_mode = "timing"
m5.instantiate()
m5.simulate()
