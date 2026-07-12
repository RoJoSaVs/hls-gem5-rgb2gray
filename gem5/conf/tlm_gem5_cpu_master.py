import m5
from m5.objects import *


system = System()

system.clk_domain = SrcClockDomain(
    clock="250MHz",
    voltage_domain=VoltageDomain("1V")
)

# Simulation start
root = Root(full_system=False, system=system)
root.system.mem_mode = "timing"
m5.instantiate()

# TODO: Set simulation time or allow for cmdline setting
m5.simulate()