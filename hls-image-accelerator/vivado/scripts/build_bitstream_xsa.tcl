# Run Vivado synthesis/implementation and export a bitstream-backed XSA.
#
# Usage from the repository root:
#   vivado -mode batch -source vivado/scripts/build_bitstream_xsa.tcl
#
# Prerequisite:
#   vivado/scripts/create_image_accel_bd.tcl must have completed successfully.
#
# Output:
#   vivado/export/image_accel_kv260_bitstream.xsa

set script_dir [file dirname [file normalize [info script]]]
set vivado_dir [file dirname $script_dir]

set project_name image_accel_kv260
set bd_name image_accel_system
set build_dir [file join $vivado_dir build]
set export_dir [file join $vivado_dir export]
set project_dir [file join $build_dir $project_name]
set project_xpr [file join $project_dir ${project_name}.xpr]
set xsa_path [file join $export_dir ${project_name}_bitstream.xsa]
set bd_path [file join $project_dir ${project_name}.srcs sources_1 bd $bd_name ${bd_name}.bd]
set wrapper_path [file join $project_dir ${project_name}.gen sources_1 bd $bd_name hdl ${bd_name}_wrapper.v]
set bit_path [file join $project_dir ${project_name}.runs impl_1 ${bd_name}_wrapper.bit]
set jobs 2

proc require_run_complete {run_name} {
    set run_obj [get_runs $run_name]
    set status [get_property STATUS $run_obj]
    if {[string first "Complete" $status] < 0} {
        puts stderr "ERROR: Vivado run $run_name did not complete. Status: $status"
        exit 1
    }
}

if {![file exists $project_xpr]} {
    puts stderr "ERROR: Vivado project not found: $project_xpr"
    puts stderr "Run first: vivado -mode batch -source vivado/scripts/create_image_accel_bd.tcl"
    exit 1
}

file mkdir $export_dir

open_project $project_xpr
open_bd_design $bd_path
validate_bd_design
save_bd_design
generate_target all [get_files $bd_path]

if {![file exists $wrapper_path]} {
    make_wrapper -files [get_files $bd_path] -top
    add_files -norecurse $wrapper_path
}

set_property top ${bd_name}_wrapper [current_fileset]
update_compile_order -fileset sources_1

reset_run synth_1
launch_runs synth_1 -jobs $jobs
wait_on_run synth_1
require_run_complete synth_1

reset_run impl_1
launch_runs impl_1 -to_step write_bitstream -jobs $jobs
wait_on_run impl_1
require_run_complete impl_1

if {![file exists $bit_path]} {
    puts stderr "ERROR: expected bitstream not found: $bit_path"
    exit 1
}

write_hw_platform -force -fixed -include_bit -file $xsa_path

puts "Bitstream: $bit_path"
puts "Exported bitstream-backed XSA: $xsa_path"
