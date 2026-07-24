# Proyectos del acelerador RGB a escala de grises

Este repositorio se divide en dos proyectos independientes:

| Proyecto | Propósito | Requisito principal |
|---|---|---|
| [`base-accelerator/`](base-accelerator/README.md) | Modelo de software original en SystemC/TLM | SystemC y CMake |
| [`hls-image-accelerator/`](hls-image-accelerator/README.md) | IP de Vitis HLS, plataforma Vivado y software bare-metal de Vitis | AMD Vitis HLS/Vivado/Vitis 2024.1 |
| [`gem5-accelerator/`](gem5-accelerator/README.md) | Co-simulacion SystemC/TLM con Gem5 | SystemC y Gem5 |

Cada proyecto tiene sus propios fuentes, resultados de compilación, scripts y
README. Los comandos de compilación y ejecución deben ejecutarse desde el
directorio del proyecto seleccionado.

## Organización del repositorio

```text
hls-gem5-rgb2gray/
├── base-accelerator/          Modelo original SystemC/TLM ejecutable en host
│   ├── src/                   CPU, bus, RAM, almacenamiento y acelerador TLM
│   ├── scripts/               Conversión entre imágenes y archivos RAW
│   └── pictures/              Imágenes de ejemplo y entradas RAW
├── hls-image-accelerator/     Implementación de hardware para KV260
│   ├── hls/                   Fuentes, testbench y automatización Vitis HLS
│   ├── vivado/                Scripts de block design, XSA y bitstream
│   ├── sw/                    Driver y aplicación bare-metal de Vitis
│   ├── docs/                  Integración Vivado y estado del proyecto
│   └── images/                Evidencia visual de ejecución en hardware
├── gem5-accelerator/          Co-simulación Gem5/SystemC/TLM
│   ├── c_program/             Programa bare-metal ARM64 y linker script
│   ├── conf/                  Configuración del sistema Gem5
│   ├── gem5_tlm_src/          Fuentes de integración Gem5-SystemC
│   └── src/                   Módulos SystemC, mapa de memoria y ejecutable
└── README.md                  Índice y organización global
```

Los directorios `build/`, `export/`, `logs/`, workspaces de Vitis y proyectos
temporales de HLS/Vivado son productos reproducibles de los scripts y están
excluidos por los archivos `.gitignore`. Se versionan las fuentes, testbenches,
scripts, documentación y evidencias añadidas en `images/`.
