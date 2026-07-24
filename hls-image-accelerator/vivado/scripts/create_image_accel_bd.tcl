# Vivado 2024.1 block-design script for the HLS RGB-to-grayscale accelerator.
#
# Usage from the repository root:
#   vivado -mode batch -source vivado/scripts/create_image_accel_bd.tcl
#
# Output:
#   vivado/build/image_accel_kv260/
#
# The script creates a block design with:
#   - Zynq UltraScale+ MPSoC Processing System
#   - HLS IP image_accel
#   - AXI SmartConnect for AXI4-Lite control
#   - AXI SmartConnect for AXI4-MM image memory traffic
#   - Processor System Reset

set script_dir [file dirname [file normalize [info script]]]
set vivado_dir [file dirname $script_dir]
set repo_root [file dirname $vivado_dir]

set project_name image_accel_kv260
set bd_name image_accel_system
set part_name xck26-sfvc784-2LV-c

set build_dir [file join $vivado_dir build]
set project_dir [file join $build_dir $project_name]
set ip_zip [file join $repo_root hls export xilinx_com_hls_image_accel_1_0.zip]
set local_ip_repo [file join $build_dir ip_repo image_accel_1_0]

if {![file exists $ip_zip]} {
    puts stderr "ERROR: HLS IP package not found: $ip_zip"
    puts stderr "Run this first from hls/: tclsh scripts/run_hls.tcl"
    exit 1
}

file mkdir $build_dir
file delete -force $local_ip_repo
file mkdir $local_ip_repo
exec unzip -oq $ip_zip -d $local_ip_repo

create_project -force $project_name $project_dir -part $part_name
set_property target_language Verilog [current_project]

set_property ip_repo_paths [list $local_ip_repo] [current_project]
update_ip_catalog

create_bd_design $bd_name
current_bd_design $bd_name

# Processing system. The explicit properties enable the PS master used for
# AXI4-Lite control and the PS slave used by PL masters to access DDR.
create_bd_cell -type ip -vlnv xilinx.com:ip:zynq_ultra_ps_e:* zynq_ultra_ps_e_0
set_property -dict [list \
    CONFIG.PSU__USE__M_AXI_GP0 {1} \
    CONFIG.PSU__USE__M_AXI_GP2 {0} \
    CONFIG.PSU__USE__S_AXI_GP0 {0} \
    CONFIG.PSU__USE__S_AXI_GP2 {1} \
    CONFIG.PSU__USE__IRQ0 {1} \
    CONFIG.PSU__CRL_APB__PL0_REF_CTRL__FREQMHZ {250} \
] [get_bd_cells zynq_ultra_ps_e_0]

# HLS accelerator.
create_bd_cell -type ip -vlnv xilinx.com:hls:image_accel:1.0 image_accel_0

# AXI interconnects.
create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:* smartconnect_ctrl
set_property -dict [list CONFIG.NUM_SI {1} CONFIG.NUM_MI {1}] \
    [get_bd_cells smartconnect_ctrl]

create_bd_cell -type ip -vlnv xilinx.com:ip:smartconnect:* smartconnect_mem
set_property -dict [list CONFIG.NUM_SI {2} CONFIG.NUM_MI {1}] \
    [get_bd_cells smartconnect_mem]

# Reset block. PS pl_resetn0 is active-low, while proc_sys_reset ext_reset_in
# is active-high, so invert it before feeding the reset controller.
create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:* rst_ps_pl_250m
create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:* rstn_to_rst
set_property -dict [list CONFIG.C_OPERATION {not} CONFIG.C_SIZE {1}] \
    [get_bd_cells rstn_to_rst]

create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:* const_one
set_property -dict [list CONFIG.CONST_WIDTH {1} CONFIG.CONST_VAL {1}] \
    [get_bd_cells const_one]

create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:* const_zero
set_property -dict [list CONFIG.CONST_WIDTH {1} CONFIG.CONST_VAL {0}] \
    [get_bd_cells const_zero]

# Clocking.
connect_bd_net [get_bd_pins zynq_ultra_ps_e_0/pl_clk0] \
    [get_bd_pins image_accel_0/ap_clk] \
    [get_bd_pins smartconnect_ctrl/aclk] \
    [get_bd_pins smartconnect_mem/aclk] \
    [get_bd_pins rst_ps_pl_250m/slowest_sync_clk] \
    [get_bd_pins zynq_ultra_ps_e_0/maxihpm0_fpd_aclk] \
    [get_bd_pins zynq_ultra_ps_e_0/saxihp0_fpd_aclk]

foreach optional_clk_pin {
    zynq_ultra_ps_e_0/maxihpm0_lpd_aclk
    zynq_ultra_ps_e_0/saxihpc0_fpd_aclk
} {
    set pin [get_bd_pins -quiet $optional_clk_pin]
    if {[llength $pin] > 0} {
        connect_bd_net [get_bd_pins zynq_ultra_ps_e_0/pl_clk0] $pin
    }
}

# Reset.
connect_bd_net [get_bd_pins zynq_ultra_ps_e_0/pl_resetn0] \
    [get_bd_pins rstn_to_rst/Op1]

connect_bd_net [get_bd_pins rstn_to_rst/Res] \
    [get_bd_pins rst_ps_pl_250m/ext_reset_in]

connect_bd_net [get_bd_pins const_one/dout] \
    [get_bd_pins rst_ps_pl_250m/dcm_locked]

connect_bd_net [get_bd_pins const_zero/dout] \
    [get_bd_pins rst_ps_pl_250m/aux_reset_in] \
    [get_bd_pins rst_ps_pl_250m/mb_debug_sys_rst]

connect_bd_net [get_bd_pins rst_ps_pl_250m/peripheral_aresetn] \
    [get_bd_pins image_accel_0/ap_rst_n] \
    [get_bd_pins smartconnect_ctrl/aresetn] \
    [get_bd_pins smartconnect_mem/aresetn]

# AXI4-Lite control path: PS master -> SmartConnect -> HLS control slave.
connect_bd_intf_net [get_bd_intf_pins zynq_ultra_ps_e_0/M_AXI_HPM0_FPD] \
    [get_bd_intf_pins smartconnect_ctrl/S00_AXI]
connect_bd_intf_net [get_bd_intf_pins smartconnect_ctrl/M00_AXI] \
    [get_bd_intf_pins image_accel_0/s_axi_control]

# AXI4-MM data path: HLS master read/write ports -> SmartConnect -> PS DDR port.
connect_bd_intf_net [get_bd_intf_pins image_accel_0/m_axi_gmem0] \
    [get_bd_intf_pins smartconnect_mem/S00_AXI]
connect_bd_intf_net [get_bd_intf_pins image_accel_0/m_axi_gmem1] \
    [get_bd_intf_pins smartconnect_mem/S01_AXI]
connect_bd_intf_net [get_bd_intf_pins smartconnect_mem/M00_AXI] \
    [get_bd_intf_pins zynq_ultra_ps_e_0/S_AXI_HP0_FPD]

# Optional interrupt path. Keep this connected so software may use either
# polling or interrupts.
connect_bd_net [get_bd_pins image_accel_0/interrupt] \
    [get_bd_pins zynq_ultra_ps_e_0/pl_ps_irq0]

assign_bd_address

# Make the control base stable for the software skeleton when possible. If this
# path changes because Vivado renames segments, assign_bd_address still leaves a
# valid address visible in Address Editor.
set ctrl_segments [get_bd_addr_segs -quiet zynq_ultra_ps_e_0/Data/SEG_image_accel_0_Reg]
if {[llength $ctrl_segments] > 0} {
    set_property offset 0xA0000000 [lindex $ctrl_segments 0]
    set_property range 64K [lindex $ctrl_segments 0]
}

validate_bd_design
save_bd_design

make_wrapper -files [get_files [file join $project_dir $project_name.srcs sources_1 bd $bd_name ${bd_name}.bd]] -top
add_files -norecurse [file join $project_dir $project_name.gen sources_1 bd $bd_name hdl ${bd_name}_wrapper.v]
update_compile_order -fileset sources_1

puts "Vivado block design created successfully."
puts "Project: $project_dir"
puts "Block design: $bd_name"
