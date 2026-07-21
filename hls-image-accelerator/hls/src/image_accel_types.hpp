#pragma once

#include <ap_int.h>
#include <cstdint>

// Image format used by the software/SystemC model:
//   - Input:  RAW RGB888, byte layout R0, G0, B0, R1, G1, B1, ...
//   - Output: RAW grayscale, one byte per pixel, byte layout Y0, Y1, ...
//
// The expected full-frame resolution is 1920x1080. The top-level accelerator
// still accepts num_pixels so software can choose the active transfer size.
#define IMAGE_ACCEL_WIDTH 1920
#define IMAGE_ACCEL_HEIGHT 1080
#define IMAGE_ACCEL_MAX_PIXELS 2073600
#define IMAGE_ACCEL_INPUT_BYTES_PER_PIXEL 3
#define IMAGE_ACCEL_MAX_RGB_BYTES 6220800

static constexpr uint32_t kImageAccelWidth = IMAGE_ACCEL_WIDTH;
static constexpr uint32_t kImageAccelHeight = IMAGE_ACCEL_HEIGHT;
static constexpr uint32_t kImageAccelMaxPixels = IMAGE_ACCEL_MAX_PIXELS;
static constexpr uint32_t kImageAccelInputBytesPerPixel =
    IMAGE_ACCEL_INPUT_BYTES_PER_PIXEL;
static constexpr uint32_t kImageAccelMaxRgbBytes = IMAGE_ACCEL_MAX_RGB_BYTES;

using image_accel_byte_t = uint8_t;
using image_accel_pixel_count_t = uint32_t;
using image_accel_channel_t = ap_uint<8>;
using image_accel_sum_t = ap_uint<18>;

struct image_accel_rgb_t {
    image_accel_channel_t r;
    image_accel_channel_t g;
    image_accel_channel_t b;
};
