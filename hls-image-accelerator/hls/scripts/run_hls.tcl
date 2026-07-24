# Vitis HLS 2024.1 automation for the RGB-to-grayscale accelerator.
#
# Usage from the hls directory:
#   tclsh scripts/run_hls.tcl
#
# This script uses the 2024.1 component/config flow through vitis-run:
#   1. C simulation
#   2. C synthesis
#   3. C/RTL co-simulation
#   4. Vivado IP package/export
#
# The generated config enables csim.code_analyzer because the classic
# vitis_hls -f csim flow can omit the design translation unit during linking
# for this component-style project.

proc run_step {description args} {
    puts ""
    puts "============================================================"
    puts $description
    puts "Command: $args"
    puts "============================================================"

    if {[catch {exec {*}$args >@ stdout 2>@ stderr} result]} {
        puts stderr ""
        puts stderr "ERROR: $description failed."
        puts stderr $result
        exit 1
    }
}

set script_dir [file dirname [file normalize [info script]]]
set hls_root [file dirname $script_dir]

set top_function image_accel
set part_name xck26-sfvc784-2LV-c

set src_dir [file join $hls_root src]
set tb_dir [file join $hls_root tb]
set build_dir [file join $hls_root build]
set export_dir [file join $hls_root export]
set work_dir [file join $build_dir image_accel_vitis_run]
set config_file [file join $build_dir hls_config_vitis_run.cfg]

set vitis_run [auto_execok vitis-run]
if {$vitis_run eq ""} {
    foreach candidate {
        /tools/Xilinx/Vitis/2024.1/bin/vitis-run
        /opt/Xilinx/Vitis/2024.1/bin/vitis-run
    } {
        if {[file executable $candidate]} {
            set vitis_run $candidate
            break
        }
    }
}

if {$vitis_run eq ""} {
    puts stderr "ERROR: Could not find vitis-run."
    puts stderr "Source the Vitis environment or set PATH to include Vitis 2024.1 bin."
    puts stderr "Example:"
    puts stderr "  source /tools/Xilinx/Vitis/2024.1/settings64.sh"
    exit 1
}

set vitis_hls [auto_execok vitis_hls]
if {$vitis_hls eq ""} {
    foreach candidate {
        /tools/Xilinx/Vitis_HLS/2024.1/bin/vitis_hls
        /opt/Xilinx/Vitis_HLS/2024.1/bin/vitis_hls
    } {
        if {[file executable $candidate]} {
            set vitis_hls $candidate
            break
        }
    }
}

if {$vitis_hls eq ""} {
    puts stderr "ERROR: Could not find vitis_hls."
    puts stderr "Source the Vitis HLS environment or set PATH to include Vitis_HLS 2024.1 bin."
    puts stderr "Example:"
    puts stderr "  source /tools/Xilinx/Vitis_HLS/2024.1/settings64.sh"
    exit 1
}

file mkdir $build_dir
file mkdir $export_dir

set cfg [open $config_file w]
puts $cfg "part=$part_name"
puts $cfg ""
puts $cfg "\[hls\]"
puts $cfg "flow_target=vivado"
puts $cfg "package.output.format=ip_catalog"
puts $cfg "package.output.syn=false"
puts $cfg "syn.top=$top_function"
puts $cfg "syn.file=[file join $src_dir image_accel.cpp]"
puts $cfg "syn.file=[file join $src_dir image_accel.hpp]"
puts $cfg "syn.file=[file join $src_dir image_accel_types.hpp]"
puts $cfg "tb.file=[file join $tb_dir image_accel_tb.c]"
puts $cfg "clock=4ns"
puts $cfg "clock_uncertainty=12.5%"
puts $cfg "csim.code_analyzer=1"
close $cfg

puts "Generated Vitis HLS config: $config_file"
puts "Build work directory:       $work_dir"
puts "Expected IP package dir:    [file join $work_dir hls impl ip]"

run_step "Running HLS C simulation" \
    $vitis_run --mode hls --csim --config $config_file --work_dir $work_dir

run_step "Running HLS C synthesis" \
    $vitis_hls -nolog -run csynth -config $config_file -work_dir $work_dir

run_step "Running HLS C/RTL co-simulation" \
    $vitis_run --mode hls --cosim --config $config_file --work_dir $work_dir

run_step "Packaging Vivado IP" \
    $vitis_run --mode hls --package --config $config_file --work_dir $work_dir

set packaged_ip [file join $work_dir hls impl ip xilinx_com_hls_image_accel_1_0.zip]
set exported_ip [file join $export_dir xilinx_com_hls_image_accel_1_0.zip]

if {[file exists $packaged_ip]} {
    file copy -force $packaged_ip $exported_ip
    puts ""
    puts "Copied Vivado IP package to: $exported_ip"
} else {
    puts ""
    puts "WARNING: Expected packaged IP was not found at: $packaged_ip"
}

puts ""
puts "HLS flow completed successfully."
