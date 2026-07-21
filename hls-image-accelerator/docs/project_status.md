# Estado del proyecto

Este documento resume el estado de los entregables del proyecto HLS RGB a
escala de grises: Vitis HLS, Vivado y Vitis.

## Completado

| Entregable | Estado | Evidencia |
|---|---|---|
| Implementacion HLS del acelerador | Completado | `hls/src/` |
| Separacion de pipeline HLS | Completado | `read_input`, `rgb_to_gray`, `write_output` |
| Interfaces AXI4-MM y AXI4-Lite | Completado | pragmas en `hls/src/image_accel.cpp` |
| Testbench HLS | Completado | `hls/tb/image_accel_tb.cpp` |
| C simulation | Completado | ejecutado con script HLS |
| C synthesis | Completado | ejecutado con script HLS |
| C/RTL co-simulation | Completado | resultado `Verilog: Pass` |
| Empaquetado como Vivado IP | Completado | `hls/export/xilinx_com_hls_image_accel_1_0.zip` |
| Script de automatizacion HLS | Completado | `hls/scripts/run_hls.tcl` |
| Guia de integracion Vivado | Completado | `docs/vivado_hls_integration.md` |
| Script de block design Vivado | Completado | `vivado/scripts/create_image_accel_bd.tcl` |
| Block design Vivado | Completado | `vivado/build/image_accel_kv260`, BD `image_accel_system` |
| Script de export XSA | Completado | `vivado/scripts/export_xsa.tcl` |
| Export de hardware/XSA | Completado | `vivado/export/image_accel_kv260.xsa` |
| Script de bitstream/XSA para hardware | Completado | `vivado/scripts/build_bitstream_xsa.tcl` |
| XSA con bitstream | Completado | `vivado/export/image_accel_kv260_bitstream.xsa`; timing cumple, WNS `1.035 ns` |
| Plantilla C bare-metal | Completado | `sw/baremetal/` |
| Script de aplicacion Vitis | Completado | `sw/scripts/create_vitis_app.tcl` |
| Aplicacion Vitis real | Completado | `sw/build/vitis_workspace/image_accel_app/Debug/image_accel_app.elf` |
| Preparacion de fuentes para Vitis | Completado | `main.c` detecta `xparameters.h` y `xil_cache.h` |
| Documentacion HLS en README | Completado | `README.md` de este proyecto |
| Resultados finales en README | Completado | incluye comandos, tamanos, hashes y artefactos generados |

## Pendiente

| Entregable | Estado | Nota |
|---|---|---|
| Ninguno para el flujo sin hardware fisico | Completado | el PDF exige KV260 como target HLS, no ejecucion obligatoria sobre tarjeta |

## Opcional

| Entregable | Estado | Nota |
|---|---|---|
| Ejecucion sobre hardware KV260 | Opcional | posible con `vivado/export/image_accel_kv260_bitstream.xsa` y `image_accel_app.elf` |

## Comandos HLS principales

Desde `hls/`:

```bash
tclsh scripts/run_hls.tcl
```

Salida esperada:

```text
hls/export/xilinx_com_hls_image_accel_1_0.zip
```

## Supuestos actuales

- La imagen de entrada es RAW RGB888.
- La resolucion objetivo es `1920 x 1080`.
- La salida es RAW grayscale de un byte por pixel.
- El IP HLS usa AXI4-MM para datos y AXI4-Lite para control.
- El target HLS es `xck26-sfvc784-2LV-c`.
- La frecuencia objetivo es `250 MHz`.
