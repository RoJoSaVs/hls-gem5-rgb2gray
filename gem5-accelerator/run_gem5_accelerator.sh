#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd -- "${SCRIPT_DIR}/.." && pwd)"

WIDTH=64
HEIGHT=64
INPUT_IMAGE=""
OUTPUT_IMAGE="output.png"
ELF_PATH="${SCRIPT_DIR}/c_program/rgb2gray-arm.elf"
GEM5_DIR=""
JOBS="$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 1)"

RAW_INPUT="${SCRIPT_DIR}/gem5-accelerator-input.raw"
RAW_OUTPUT="${SCRIPT_DIR}/output.raw"

declare -a SELECTED_STEPS=()

usage()
{
    cat <<'EOF'
Usage:
  run_gem5_accelerator.sh [options] [step options]

Required for a complete run:
  -i, --input-image PATH   Input JPEG image file, will be resized to (64x64) if no width or height is set
  -g, --gem5-dir PATH      gem5 source directory

Other options:
  -h, --help               Print this help text
  -e, --elf                ELF file output/input name + path
  -j, --jobs COUNT         Parallel build jobs (default: available CPUs)
  -o, --output-image PATH  Output grayscale image path (for example output.png)
  -w, --width PIXELS       Image width (default: 64)
  -H, --height PIXELS      Image height (default: 64)

Step options:
      --build-simulator    Build the SystemC/gem5 accelerator executable
      --build-m5           Build the ARM64 libm5.a library
      --build-elf          Cross-compile the bare-metal C program
      --prepare-input      Resize/convert the input image to raw RGB
      --configure          Generate m5out/config.ini
      --run                Run the SystemC/gem5 simulation
      --prepare-output     Convert output.raw to the requested image
      --all                Run every step above in pipeline order

If no step option is supplied, --all is used. Multiple step options may be
combined and are always executed in pipeline order.

Examples:
  # Complete build and run
  ./run_gem5_accelerator.sh -w 1920 -H 1080 \
      -i ../base-accelerator/pictures/input.png \
      -o results/output.png -e c_program/rgb2gray-arm.elf \
      -g /home/user/gem5

  # Rebuild only the ELF
  ./run_gem5_accelerator.sh -w 640 -H 480 \
      -e c_program/rgb2gray-arm.elf -g /home/user/gem5 --build-elf

  # Run using an existing configuration, then convert its output
  ./run_gem5_accelerator.sh -w 640 -H 480 -o results/output.png \
      --run --prepare-output
EOF
}

die()
{
    echo "error: $*" >&2
    exit 1
}

add_step()
{
    SELECTED_STEPS+=("$1")
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -w|--width)
            WIDTH="$2"
            shift 2
            ;;
        -H|--height)
            HEIGHT="$2"
            shift 2
            ;;
        -i|--input-image)
            INPUT_IMAGE="$2"
            shift 2
            ;;
        -o|--output-image)
            OUTPUT_IMAGE="$2"
            shift 2
            ;;
        -e|--elf)
            ELF_PATH="$2"
            shift 2
            ;;
        -g|--gem5-dir)
            GEM5_DIR="$2"
            shift 2
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        --build-simulator|--build-m5|--build-elf|--prepare-input|--configure|--run|--prepare-output)
            add_step "${1#--}"
            shift
            ;;
        --all)
            SELECTED_STEPS=(build-simulator build-m5 build-elf prepare-input configure run prepare-output)
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        --)
            shift
            [[ $# -eq 0 ]] || die "unexpected positional argument: $1"
            ;;
        -*)
            die "unknown option: $1 (use --help for usage)"
            ;;
        *)
            die "unexpected positional argument: $1 (use --help for usage)"
            ;;
    esac
done

[[ "$WIDTH" =~ ^[1-9][0-9]*$ ]] || die "width must be a positive integer"
[[ "$HEIGHT" =~ ^[1-9][0-9]*$ ]] || die "height must be a positive integer"
[[ "$JOBS" =~ ^[1-9][0-9]*$ ]] || die "jobs must be a positive integer"

if [[ ${#SELECTED_STEPS[@]} -eq 0 ]]; then
    SELECTED_STEPS=(build-simulator build-m5 build-elf prepare-input configure run prepare-output)
fi

step_selected()
{
    local wanted="$1"
    local selected
    for selected in "${SELECTED_STEPS[@]}"; do
        [[ "$selected" == "$wanted" ]] && return 0
    done
    return 1
}

if step_selected prepare-input || step_selected configure; then
    [[ -n "$INPUT_IMAGE" ]] || die "--input-image is required for input preparation/configuration"
fi

if step_selected prepare-output; then
    [[ -n "$OUTPUT_IMAGE" ]] || die "--output-image is required for output preparation"
fi

if step_selected build-simulator || step_selected build-m5 || \
   step_selected build-elf || step_selected configure; then
    [[ -d "$GEM5_DIR" ]] || die "gem5_dir is required for building"
fi

if step_selected build-simulator; then
    echo "==> Building SystemC/gem5 accelerator files"
    scons -C "$SCRIPT_DIR" "gem5_root=$GEM5_DIR" "-j$JOBS"
fi

if step_selected build-m5; then
    echo "==> Building ARM64 libm5 cross compile library"
    scons -C "$GEM5_DIR/util/m5" \
        arm64.CROSS_COMPILE=aarch64-linux-gnu- \
        build/arm64/out/libm5.a \
        "-j$JOBS"
fi

if step_selected build-elf; then
    echo "==> Building bare-metal ELF: $ELF_PATH"
    mkdir -p "$(dirname -- "$ELF_PATH")"
    aarch64-linux-gnu-gcc \
        -march=armv8-a \
        -O2 \
        -ffreestanding \
        -fno-builtin \
        -nostdlib \
        -static \
        "-DIMAGE_WIDTH=$WIDTH" \
        "-DIMAGE_HEIGHT=$HEIGHT" \
        -T "${SCRIPT_DIR}/c_program/linker.ld" \
        -I"${GEM5_DIR}/include" \
        "${SCRIPT_DIR}/c_program/start.S" \
        "${SCRIPT_DIR}/c_program/convert_to_gray.c" \
        "${GEM5_DIR}/util/m5/build/arm64/out/libm5.a" \
        -o "$ELF_PATH"
fi

if step_selected prepare-input; then
    [[ -f "$INPUT_IMAGE" ]] || die "input image does not exist: $INPUT_IMAGE"
    echo "==> Converting input image to ${WIDTH}x${HEIGHT} RGB raw"
    python3 "${REPO_DIR}/base-accelerator/scripts/image_to_raw.py" \
        "$INPUT_IMAGE" "$RAW_INPUT" \
        --width "$WIDTH" \
        --height "$HEIGHT" \
        --resize
fi

if step_selected configure; then
    [[ -f "$ELF_PATH" ]] || die "ELF does not exist: $ELF_PATH"
    [[ -f "$RAW_INPUT" ]] || die "raw input does not exist: $RAW_INPUT (run --prepare-input first)"
    echo "==> Generating gem5 configuration"
    mkdir -p "${SCRIPT_DIR}/m5out"
    configure_status=0
    "$GEM5_DIR/build/ARM/gem5.opt" \
        --outdir="${SCRIPT_DIR}/m5out" \
        "${SCRIPT_DIR}/conf/tlm_gem5_cpu_master.py" \
        --binary="$ELF_PATH" \
        --image="$RAW_INPUT" || configure_status=$?

    if [[ "$configure_status" -ne 0 ]]; then
        if [[ -f "${SCRIPT_DIR}/m5out/config.ini" ]]; then
            echo "warning: gem5 configuration exited with status $configure_status;" \
                 "continuing because m5out/config.ini was generated" >&2
        else
            die "gem5 configuration failed with status $configure_status and did not generate m5out/config.ini"
        fi
    fi
fi

if step_selected run; then
    [[ -x "${SCRIPT_DIR}/build/accelerator/gem5.sc" ]] || \
        die "simulator is missing; run --build-simulator first"
    [[ -f "${SCRIPT_DIR}/m5out/config.ini" ]] || \
        die "m5out/config.ini is missing; run --configure first"
    echo "==> Running simulation"
    (
        cd "$SCRIPT_DIR"
        build/accelerator/gem5.sc m5out/config.ini -v
    )
fi

if step_selected prepare-output; then
    [[ -f "$RAW_OUTPUT" ]] || die "simulation output does not exist: $RAW_OUTPUT"
    echo "==> Converting raw grayscale output to: $OUTPUT_IMAGE"
    python3 "${REPO_DIR}/base-accelerator/scripts/raw_to_image.py" \
        "$RAW_OUTPUT" "$OUTPUT_IMAGE" \
        --width "$WIDTH" \
        --height "$HEIGHT" \
        --mode gray
fi

echo "==> Requested steps completed"
