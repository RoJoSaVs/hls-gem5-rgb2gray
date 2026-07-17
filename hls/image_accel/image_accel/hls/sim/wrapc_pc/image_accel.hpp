#pragma once

#include "image_accel_types.hpp"

// Vitis HLS top function.
//
// Control behavior:
//   - input_rgb and output_gray base addresses are written through AXI4-Lite.
//   - num_pixels is written through AXI4-Lite.
//   - ap_ctrl_hs return/control is exposed on the same AXI4-Lite bundle.
//
// Memory behavior:
//   - input_rgb is an AXI4 memory-mapped read port carrying RAW RGB888 data.
//   - output_gray is an AXI4 memory-mapped write port carrying one grayscale
//     byte per input pixel.
extern "C" void image_accel(
    volatile const image_accel_byte_t* input_rgb,
    volatile image_accel_byte_t* output_gray,
    image_accel_pixel_count_t num_pixels);

