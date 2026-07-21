#include "image_accel_control.h"

#include <stdint.h>

#if defined(__has_include)
#if __has_include("xparameters.h")
#include "xparameters.h"
#endif
#if __has_include("xil_cache.h")
#include "xil_cache.h"
#define IMAGE_ACCEL_HAS_XIL_CACHE 1
#endif
#endif

// Replace these addresses with values from the exported Vivado/Vitis platform.
// In a Vitis standalone BSP, the accelerator base address is usually available
// in xparameters.h as XPAR_<INSTANCE>_S_AXI_CONTROL_BASEADDR.
#ifndef IMAGE_ACCEL_BASE_ADDR
#if defined(XPAR_IMAGE_ACCEL_0_S_AXI_CONTROL_BASEADDR)
#define IMAGE_ACCEL_BASE_ADDR XPAR_IMAGE_ACCEL_0_S_AXI_CONTROL_BASEADDR
#elif defined(XPAR_XIMAGE_ACCEL_0_S_AXI_CONTROL_BASEADDR)
#define IMAGE_ACCEL_BASE_ADDR XPAR_XIMAGE_ACCEL_0_S_AXI_CONTROL_BASEADDR
#else
#define IMAGE_ACCEL_BASE_ADDR 0xA0000000u
#endif
#endif

// Example DDR buffer addresses. These must point to valid, non-overlapping
// physical DDR regions large enough for the full image.
#ifndef IMAGE_ACCEL_INPUT_ADDR
#define IMAGE_ACCEL_INPUT_ADDR 0x10000000ull
#endif

#ifndef IMAGE_ACCEL_OUTPUT_ADDR
#define IMAGE_ACCEL_OUTPUT_ADDR 0x10600000ull
#endif

#ifndef IMAGE_ACCEL_TIMEOUT
#define IMAGE_ACCEL_TIMEOUT 100000000u
#endif

// Platform hook: flush input cache lines before accelerator reads from DDR.
// Replace with Xil_DCacheFlushRange() or platform-specific cache maintenance
// when integrating in a real standalone application.
static void flush_input_buffer(uint64_t addr, uint32_t size_bytes)
{
#if defined(IMAGE_ACCEL_HAS_XIL_CACHE)
    Xil_DCacheFlushRange((INTPTR)addr, size_bytes);
#else
    (void)addr;
    (void)size_bytes;
#endif
}

// Platform hook: invalidate output cache lines after accelerator writes to DDR.
// Replace with Xil_DCacheInvalidateRange() or platform-specific cache
// maintenance when integrating in a real standalone application.
static void invalidate_output_buffer(uint64_t addr, uint32_t size_bytes)
{
#if defined(IMAGE_ACCEL_HAS_XIL_CACHE)
    Xil_DCacheInvalidateRange((INTPTR)addr, size_bytes);
#else
    (void)addr;
    (void)size_bytes;
#endif
}

int main(void)
{
    const uintptr_t accel_base = (uintptr_t)IMAGE_ACCEL_BASE_ADDR;
    const uint64_t input_addr = (uint64_t)IMAGE_ACCEL_INPUT_ADDR;
    const uint64_t output_addr = (uint64_t)IMAGE_ACCEL_OUTPUT_ADDR;

    // Before this point, software must copy a 1920x1080 RAW RGB888 image into
    // input_addr. The accelerator writes one grayscale byte per pixel to
    // output_addr.
    flush_input_buffer(input_addr, IMAGE_ACCEL_RGB_BYTES);

    const int status = image_accel_run(accel_base,
                                       input_addr,
                                       output_addr,
                                       IMAGE_ACCEL_NUM_PIXELS,
                                       IMAGE_ACCEL_TIMEOUT);
    if (status != 0) {
        return 1;
    }

    invalidate_output_buffer(output_addr, IMAGE_ACCEL_GRAY_BYTES);

    // At this point output_addr contains IMAGE_ACCEL_GRAY_BYTES bytes of RAW
    // grayscale image data.
    return 0;
}
