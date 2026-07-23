# Accelerator Gem5 Co-simulation

## Project layout

| Path | Purpose |
|---|---|
| `conf` | Gem5 system configuration generating script |
| `c_program` | Bare metal C program files |
| `gem5_tlm_src` | Script for copying gem5-SystemC bridge sources |
| `src` | SystemC module sources |

## Requirements

### Dependencies

For Ubuntu or Debian:

```bash
sudo apt update
sudo apt install scons build-essential git m4 zlib1g zlib1g-dev \
    libprotobuf-dev protobuf-compiler libprotoc-dev \
    libgoogle-perftools-dev python3-dev
```

Cross-compiling the C program requires aarch64 toolchain:
```bash
sudo apt install gcc-aarch64-linux-gnu binutils-aarch64-linux-gnu
```

### Pillow

Image conversion scripts use Pillow:

```bash
python3 -m pip install pillow
```

### GEM5

Clone the repo (this was run on the v25.1.0.1)
```bash
git clone -b v25.1.0.1 https://github.com/gem5/gem5.git
```

Gem5 must be built for ARM before building simulation. Also build 
gem5 as a library with cxx-config support and (optionally) without python:
```bash
SIM_DIR=$PWD
GEM5_DIR=/path/to/gem5/repo/
cd $GEM5_DIR
scons build/ARM/gem5.opt -j$(nproc)
scons setconfig build/ARM USE_SYSTEMC=n
scons --with-cxx-config --without-python --without-tcmalloc \
      --duplicate-sources build/ARM/libgem5_opt.so -j$(nproc)
cd $SIM_DIR
```

## Building and Running Simulation

### Specific commands

Build the  SystemC Gem5 files:
```bash
scons gem5_root=$GEM5_DIR
```

Build the needed m5 library for C cross-compile: 
```bash
scons \
  -C $GEM5_DIR/util/m5 \
  arm64.CROSS_COMPILE=aarch64-linux-gnu- \
  build/arm64/out/libm5.a
```

Build the ARM64 bare metal C program for simulation
```bash
aarch64-linux-gnu-gcc \
  -march=armv8-a \
  -O2 -ffreestanding \
  -fno-builtin -nostdlib \
  -static -DIMAGE_WIDTH=64 -DIMAGE_HEIGHT=64 \
  -T c_program/linker.ld \
  -I$GEM5_DIR/include \
  c_program/start.S \
  c_program/convert_to_gray.c \
  $GEM5_DIR/util/m5/build/arm64/out/libm5.a \
  -o c_program/rgb2gray-arm.elf
```

Update the gem5 modules config:
```bash
$GEM5_DIR/build/ARM/gem5.opt   --outdir=m5out   conf/tlm_gem5_cpu_master.py  \
      --binary=c_program/rgb2gray-arm.elf   --image=grump-small.raw
```

Run simulation:
```bash
build/accelerator/gem5.sc m5out/config.ini -v
```

## Memory layout
| Base | Size | Purpose |
|---|---|---|
| 0x00000000 | 64MiB | RAM |
| 0x10000000 | 256MiB | Accelerator Config port |
| 0x20000000 | 1000MiB | Persistent Memory 'NVMe' |

## Accelerator register map
| Address | Purpose |
|---|---|
| 0x00 | RAM RGB byte data location address |
| 0x08 | RAM GRAY byte data location address |
| 0x10 | Pixel amount to read config |
| 0x14 | Accelerator command config |
| 0x18 | Accelerator status |

###  Accelerator Status Flags
| Flag | Purpose |
|---|---|
| 0 | STATUS_IDLE |
| 1 | STATUS_BUSY |
| 2 | STATUS_DONE |
| 3 | STATUS_ERROR |
