# Integracion Vivado del IP `image_accel`

Esta carpeta contiene el script para crear el block design Vivado del
acelerador HLS `image_accel`.

## Requisito previo

Primero genera el IP HLS:

```bash
cd hls
tclsh scripts/run_hls.tcl
cd ..
```

Debe existir:

```text
hls/export/xilinx_com_hls_image_accel_1_0.zip
```

## Crear el block design

Desde la raiz de este proyecto HLS:

```bash
vivado -mode batch -source vivado/scripts/create_image_accel_bd.tcl
```

Si `vivado` no esta en el `PATH`, usa la ruta completa:

```bash
/tools/Xilinx/Vivado/2024.1/bin/vivado -mode batch -source vivado/scripts/create_image_accel_bd.tcl
```

El proyecto se crea en:

```text
vivado/build/image_accel_kv260/
```

El block design generado se llama:

```text
image_accel_system
```

## Contenido del block design

El script instancia:

| Bloque | Uso |
|---|---|
| `zynq_ultra_ps_e_0` | Processing System del Zynq UltraScale+ |
| `image_accel_0` | IP HLS RGB a grayscale |
| `smartconnect_ctrl` | conexion AXI4-Lite de control |
| `smartconnect_mem` | conexion AXI4-MM hacia DDR |
| `rst_ps_pl_250m` | reset sincronizado para PL |

## Conexiones principales

| Origen | Destino | Proposito |
|---|---|---|
| `M_AXI_HPM0_FPD` | `s_axi_control` | control AXI4-Lite del acelerador |
| `m_axi_gmem0` | `S_AXI_HP0_FPD` | lectura RGB desde DDR |
| `m_axi_gmem1` | `S_AXI_HP0_FPD` | escritura grayscale hacia DDR |
| `pl_clk0` | `ap_clk` | reloj del acelerador |
| `peripheral_aresetn` | `ap_rst_n` | reset activo bajo |
| `interrupt` | `pl_ps_irq0` | interrupcion opcional hacia PS |

## Nota

Este script crea una base reproducible para el block design. Antes de generar
bitstream para una tarjeta real, revisa en Vivado:

- configuracion del PS para la plataforma KV260 usada
- reloj real conectado a PL
- mapa de direcciones en Address Editor
- conexiones DDR habilitadas para trafico desde PL
- asignacion final de interrupciones

La ejecucion batch valida el block design y genera el HDL wrapper. Vivado puede
mostrar advertencias de ancho `AWUSER/ARUSER` o `ARLOCK/AWLOCK` entre el IP HLS
y SmartConnect; con la configuracion actual no bloquean la validacion del
diseno.

## Exportar hardware/XSA

Despues de crear el block design:

```bash
vivado -mode batch -source vivado/scripts/export_xsa.tcl
```

O con ruta completa:

```bash
/tools/Xilinx/Vivado/2024.1/bin/vivado -mode batch -source vivado/scripts/export_xsa.tcl
```

La salida esperada es:

```text
vivado/export/image_accel_kv260.xsa
```

El script exporta la plataforma de hardware sin ejecutar bitstream completo.
Esto es suficiente para crear una aplicacion Vitis y obtener `xparameters.h`.

## Generar bitstream y XSA para hardware

Para ejecutar en la tarjeta KV260 se necesita un XSA que incluya bitstream. Ese
flujo tarda mas porque ejecuta synthesis e implementation:

```bash
vivado -mode batch -source vivado/scripts/build_bitstream_xsa.tcl
```

O con ruta completa:

```bash
/tools/Xilinx/Vivado/2024.1/bin/vivado -mode batch -source vivado/scripts/build_bitstream_xsa.tcl
```

La salida esperada es:

```text
vivado/export/image_accel_kv260_bitstream.xsa
```

Usa este XSA en Vitis cuando el objetivo sea programar la FPGA y ejecutar la
aplicacion en hardware real.
