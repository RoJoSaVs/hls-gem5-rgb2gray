# Export hardware platform (XSA) from the generated Vivado block design.
#
# Usage from the repository root:
#   vivado -mode batch -source vivado/scripts/export_xsa.tcl
#
# Prerequisite:
#   vivado/scripts/create_image_accel_bd.tcl must have completed successfully.
#
# Output:
#   vivado/export/image_accel_kv260.xsa

set script_dir [file dirname [file normalize [info script]]]
set vivado_dir [file dirname $script_dir]

set project_name image_accel_kv260
set bd_name image_accel_system
set build_dir [file join $vivado_dir build]
set export_dir [file join $vivado_dir export]
set project_dir [file join $build_dir $project_name]
set project_xpr [file join $project_dir ${project_name}.xpr]
set xsa_path [file join $export_dir ${project_name}.xsa]
set bd_path [file join $project_dir ${project_name}.srcs sources_1 bd $bd_name ${bd_name}.bd]
set wrapper_path [file join $project_dir ${project_name}.gen sources_1 bd $bd_name hdl ${bd_name}_wrapper.v]

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

# Export the hardware description without bitstream. This is sufficient to
# create a Vitis platform/application and inspect xparameters.h/register maps.
write_hw_platform -force -fixed -file $xsa_path

puts "Exported XSA: $xsa_path"
