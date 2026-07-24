# Create and build a standalone Vitis application for the HLS image_accel IP.
#
# Usage from the repository root:
#   xsct sw/scripts/create_vitis_app.tcl
#
# Prerequisite:
#   vivado/export/image_accel_kv260_bitstream.xsa should exist for hardware.
#   The script falls back to vivado/export/image_accel_kv260.xsa for app-only
#   builds.
#
# Output:
#   sw/build/vitis_workspace/image_accel_app/

set script_dir [file dirname [file normalize [info script]]]
set sw_dir [file dirname $script_dir]
set repo_root [file dirname $sw_dir]

set workspace [file join $sw_dir build vitis_workspace]
set app_name image_accel_app
set xsa_bitstream_path [file join $repo_root vivado export image_accel_kv260_bitstream.xsa]
set xsa_nobit_path [file join $repo_root vivado export image_accel_kv260.xsa]
set xsa_path $xsa_bitstream_path
set src_dir [file join $sw_dir baremetal]
set proc_name psu_cortexa53_0
set os_name standalone
set template_name "Empty Application(C)"

if {![file exists $xsa_path]} {
    if {[file exists $xsa_nobit_path]} {
        set xsa_path $xsa_nobit_path
        puts "WARNING: bitstream XSA not found; using app-only XSA: $xsa_path"
    } else {
        puts stderr "ERROR: XSA not found: $xsa_bitstream_path"
        puts stderr "Also missing fallback XSA: $xsa_nobit_path"
        puts stderr "Run first: vivado -mode batch -source vivado/scripts/build_bitstream_xsa.tcl"
        exit 1
    }
}

if {![file exists $src_dir]} {
    puts stderr "ERROR: source directory not found: $src_dir"
    exit 1
}

file mkdir [file dirname $workspace]

# Recreate only the generated software workspace. Source files remain in
# sw/baremetal and are not deleted.
file delete -force $workspace
setws $workspace

app create \
    -name $app_name \
    -hw $xsa_path \
    -proc $proc_name \
    -os $os_name \
    -template $template_name

importsources -name $app_name -path $src_dir
app build -name $app_name

puts "Vitis standalone application created successfully."
puts "Workspace: $workspace"
puts "Application: $app_name"
puts "Hardware XSA: $xsa_path"
