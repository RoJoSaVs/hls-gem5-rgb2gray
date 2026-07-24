# Ejemplo bare-metal de control para `image_accel`

Esta carpeta contiene un driver C minimo a nivel de registros y una aplicacion
de ejemplo para controlar el IP `image_accel` generado por HLS.

## Archivos

| Archivo | Uso |
|---|---|
| `image_accel_control.h` | offsets de registros, mascaras de bits y API del driver |
| `image_accel_control.c` | implementacion del driver MMIO AXI4-Lite |
| `main.c` | plantilla de aplicacion embebida |

## Valores requeridos de la plataforma

Actualiza estos valores con los datos de la plataforma de hardware exportada
desde Vivado/Vitis:

```c
#define IMAGE_ACCEL_BASE_ADDR  0xA0000000u
#define IMAGE_ACCEL_INPUT_ADDR 0x10000000ull
#define IMAGE_ACCEL_OUTPUT_ADDR 0x10600000ull
```

`IMAGE_ACCEL_BASE_ADDR` es la direccion base AXI4-Lite asignada a
`s_axi_control`.

`IMAGE_ACCEL_INPUT_ADDR` y `IMAGE_ACCEL_OUTPUT_ADDR` deben ser direcciones
fisicas validas en DDR. No deben traslaparse.

## Tamanos de buffers

Para una imagen completa de 1080p:

```text
input RGB bytes  = 6,220,800
output gray bytes = 2,073,600
num_pixels        = 2,073,600
```

## Mantenimiento de cache

Antes de iniciar el acelerador:

```c
flush input buffer
```

Despues de `AP_DONE`:

```c
invalidate output buffer
```

En un BSP standalone de Xilinx, reemplaza las funciones hook de `main.c` por:

```c
Xil_DCacheFlushRange(input_addr, IMAGE_ACCEL_RGB_BYTES);
Xil_DCacheInvalidateRange(output_addr, IMAGE_ACCEL_GRAY_BYTES);
```

La plantilla `main.c` ya intenta incluir `xil_cache.h` automaticamente cuando
esta disponible en el BSP de Vitis. En ese caso usa esas funciones directamente.

## Crear la aplicacion Vitis

Desde la raiz de este proyecto HLS, despues de exportar el XSA:

```bash
xsct sw/scripts/create_vitis_app.tcl
```

Si `xsct` no esta en el `PATH`, usa:

```bash
/tools/Xilinx/Vitis/2024.1/bin/xsct sw/scripts/create_vitis_app.tcl
```

En algunas instalaciones Linux, `xsct` falla antes de ejecutar el script si no
encuentra `xlsclients`:

```text
ERROR: xlsclients is not available on the system
```

En Fedora normalmente se resuelve instalando el paquete que provee esa utilidad,
por ejemplo `xorg-x11-utils`, y volviendo a abrir la terminal de Vitis.

La aplicacion se genera en:

```text
sw/build/vitis_workspace/image_accel_app/
```

El script usa:

```text
XSA preferido: vivado/export/image_accel_kv260_bitstream.xsa
XSA fallback:  vivado/export/image_accel_kv260.xsa
Procesador: psu_cortexa53_0
Sistema operativo: standalone
Plantilla: Empty Application(C)
```

Usa el XSA con bitstream cuando la aplicacion se vaya a ejecutar sobre la
tarjeta. El XSA sin bitstream sirve para construir la aplicacion y revisar el
BSP, pero no programa la logica PL.

## Secuencia de registros

El driver escribe:

```text
0x10 input_rgb low
0x14 input_rgb high
0x1c output_gray low
0x20 output_gray high
0x28 num_pixels
0x00 AP_START
```

Luego hace polling del bit `AP_DONE` en el registro `0x00`.
