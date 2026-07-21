# HLS RGB-to-Grayscale Accelerator

This project contains the hardware implementation of `image_accel`, its HLS
testbench, Vivado integration scripts, and a Vitis bare-metal control example.
It is independent of the SystemC/TLM model in the sibling project.

## Design summary

| Item | Value |
|---|---|
| Top function | `image_accel` |
| Target | AMD Kria KV260 / K26 SOM, `xck26-sfvc784-2LV-c` |
| Clock target | 250 MHz, 4 ns period |
| Input | RAW RGB888, `R, G, B`, three bytes per pixel |
| Output | RAW grayscale, one byte per pixel |
| Full frame | 1920x1080, 2,073,600 pixels |
| Formula | `(77 * R + 150 * G + 29 * B) >> 8` |

The HLS design is split into three dataflow stages:

1. `read_input`: AXI4 memory-mapped RGB reads.
2. `rgb_to_gray`: pipelined grayscale conversion.
3. `write_output`: AXI4 memory-mapped grayscale writes.

`hls::stream`, `DATAFLOW`, and loop `PIPELINE` pragmas connect and pipeline
the stages.

## Project layout

| Path | Purpose |
|---|---|
| `hls/src/` | Synthesizable top function and HLS types |
| `hls/tb/` | C++ testbench for C simulation and C/RTL co-simulation |
| `hls/scripts/` | Automated Vitis HLS flow |
| `vivado/scripts/` | Block-design, XSA, and bitstream automation |
| `sw/baremetal/` | AXI4-Lite register driver and standalone application |
| `sw/scripts/` | XSCT application-build script |
| `docs/` | Integration guide and project status |

## Requirements

- AMD Vitis HLS 2024.1
- AMD Vivado 2024.1
- AMD Vitis/XSCT 2024.1
- Target part: `xck26-sfvc784-2LV-c`

The scripts look for the standard AMD tools in `PATH` and then in
`/tools/Xilinx/.../2024.1/bin`. If needed, load the AMD environment first:

```bash
source /tools/Xilinx/Vitis/2024.1/settings64.sh
source /tools/Xilinx/Vitis_HLS/2024.1/settings64.sh
source /tools/Xilinx/Vivado/2024.1/settings64.sh
```

## Run the HLS flow

From this project directory:

```bash
cd hls
tclsh scripts/run_hls.tcl
```

The script performs C simulation, C synthesis, Verilog C/RTL co-simulation,
and IP packaging. Expected results:

```text
HLS C simulation: pass
HLS C synthesis: pass
C/RTL co-simulation: Verilog: Pass
Package/export: pass
```

The packaged IP is written to:

```text
hls/export/xilinx_com_hls_image_accel_1_0.zip
```

The testbench checks known RGB values and a deterministic 1024-pixel pattern
against the integer grayscale formula. Successful simulation prints:

```text
image_accel testbench passed
```

## Create the Vivado platform

Run these commands from this project directory, in order:

```bash
vivado -mode batch -source vivado/scripts/create_image_accel_bd.tcl
vivado -mode batch -source vivado/scripts/export_xsa.tcl
vivado -mode batch -source vivado/scripts/build_bitstream_xsa.tcl
```

Expected artifacts:

```text
vivado/build/image_accel_kv260/
vivado/export/image_accel_kv260.xsa
vivado/export/image_accel_kv260_bitstream.xsa
```

The block design connects the HLS IP AXI4-Lite control interface to the PS and
the two AXI4 memory-mapped master ports to DDR through SmartConnect.

## Create the Vitis application

After exporting an XSA, build the standalone application:

```bash
xsct sw/scripts/create_vitis_app.tcl
```

Expected output:

```text
sw/build/vitis_workspace/image_accel_app/Debug/image_accel_app.elf
```

The application programs input/output DDR addresses, `num_pixels`, and
`AP_START`; it then waits for `AP_DONE`. The generated control base address is
`0xA0000000` in the validated design.

## IP interfaces

| Interface | Type | Purpose |
|---|---|---|
| `s_axi_control` | AXI4-Lite slave | control registers and pointer arguments |
| `m_axi_gmem0` | AXI4 memory-mapped master | RGB input reads |
| `m_axi_gmem1` | AXI4 memory-mapped master | grayscale output writes |
| `ap_clk` | clock | 250 MHz target clock |
| `ap_rst_n` | active-low reset | accelerator reset |
| `interrupt` | interrupt | optional completion interrupt |

The relevant AXI4-Lite offsets are `0x10`/`0x14` for `input_rgb`,
`0x1c`/`0x20` for `output_gray`, and `0x28` for `num_pixels`.

For a full 1080p image:

```text
input RGB bytes  = 6,220,800
output gray bytes = 2,073,600
num_pixels        = 2,073,600
```

See `docs/vivado_hls_integration.md` for the full Vivado connection and
register-map reference, and `sw/baremetal/README.md` for the software API.

