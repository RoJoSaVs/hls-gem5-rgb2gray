#include "image_accel.hpp"

#include <hls_stream.h>

namespace {

using rgb_stream_t = hls::stream<image_accel_rgb_t>;
using gray_stream_t = hls::stream<image_accel_channel_t>;

// Stage 1: read RGB888 bytes from AXI memory and emit one RGB pixel per loop.
static void read_input(
    volatile const image_accel_byte_t* input_rgb,
    rgb_stream_t& rgb_stream,
    image_accel_pixel_count_t num_pixels)
{
#pragma HLS INLINE off
read_loop:
    for (image_accel_pixel_count_t i = 0; i < num_pixels; ++i) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=IMAGE_ACCEL_MAX_PIXELS avg=IMAGE_ACCEL_MAX_PIXELS
#pragma HLS PIPELINE II=1
        const image_accel_pixel_count_t base =
            i * IMAGE_ACCEL_INPUT_BYTES_PER_PIXEL;

        image_accel_rgb_t pixel;
        pixel.r = static_cast<image_accel_channel_t>(input_rgb[base]);
        pixel.g = static_cast<image_accel_channel_t>(input_rgb[base + 1u]);
        pixel.b = static_cast<image_accel_channel_t>(input_rgb[base + 2u]);

        rgb_stream.write(pixel);
    }
}

// Existing accelerator formula preserved from src/accelerator.h:
//   BT.601 integer approximation, Y = (77R + 150G + 29B) >> 8.
static image_accel_channel_t grayscale_from_rgb(const image_accel_rgb_t& pixel)
{
#pragma HLS INLINE
    const image_accel_sum_t weighted_sum =
        static_cast<image_accel_sum_t>(77u) * pixel.r +
        static_cast<image_accel_sum_t>(150u) * pixel.g +
        static_cast<image_accel_sum_t>(29u) * pixel.b;

    return static_cast<image_accel_channel_t>(weighted_sum >> 8);
}

// Stage 2: convert RGB pixels to one-byte grayscale pixels.
static void rgb_to_gray(
    rgb_stream_t& rgb_stream,
    gray_stream_t& gray_stream,
    image_accel_pixel_count_t num_pixels)
{
#pragma HLS INLINE off
process_loop:
    for (image_accel_pixel_count_t i = 0; i < num_pixels; ++i) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=IMAGE_ACCEL_MAX_PIXELS avg=IMAGE_ACCEL_MAX_PIXELS
#pragma HLS PIPELINE II=1
        const image_accel_rgb_t pixel = rgb_stream.read();
        gray_stream.write(grayscale_from_rgb(pixel));
    }
}

// Stage 3: write one grayscale byte per pixel to AXI memory.
static void write_output(
    volatile image_accel_byte_t* output_gray,
    gray_stream_t& gray_stream,
    image_accel_pixel_count_t num_pixels)
{
#pragma HLS INLINE off
write_loop:
    for (image_accel_pixel_count_t i = 0; i < num_pixels; ++i) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=IMAGE_ACCEL_MAX_PIXELS avg=IMAGE_ACCEL_MAX_PIXELS
#pragma HLS PIPELINE II=1
        output_gray[i] =
            static_cast<image_accel_byte_t>(gray_stream.read());
    }
}

} // namespace

extern "C" void image_accel(
    volatile const image_accel_byte_t* input_rgb,
    volatile image_accel_byte_t* output_gray,
    image_accel_pixel_count_t num_pixels)
{
// AXI4 memory-mapped data interfaces. Separate bundles allow concurrent reads
// and writes when the platform connects them to independent/interleaved memory
// paths. Pointer values are supplied as AXI4-Lite slave offsets.
#pragma HLS INTERFACE m_axi port=input_rgb offset=slave bundle=gmem0 depth=IMAGE_ACCEL_MAX_RGB_BYTES max_read_burst_length=64 num_read_outstanding=16
#pragma HLS INTERFACE m_axi port=output_gray offset=slave bundle=gmem1 depth=IMAGE_ACCEL_MAX_PIXELS max_write_burst_length=64 num_write_outstanding=16

// AXI4-Lite control register bundle. Vitis HLS will expose input_rgb,
// output_gray, num_pixels, and ap_ctrl_hs return/control registers here.
#pragma HLS INTERFACE s_axilite port=input_rgb bundle=control
#pragma HLS INTERFACE s_axilite port=output_gray bundle=control
#pragma HLS INTERFACE s_axilite port=num_pixels bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

// Pipeline organization:
//   read_input -> rgb_to_gray -> write_output
// Streams decouple the stages so HLS can schedule them concurrently.
#pragma HLS DATAFLOW

    rgb_stream_t rgb_stream("rgb_stream");
    gray_stream_t gray_stream("gray_stream");
#pragma HLS STREAM variable=rgb_stream depth=64
#pragma HLS STREAM variable=gray_stream depth=64

    read_input(input_rgb, rgb_stream, num_pixels);
    rgb_to_gray(rgb_stream, gray_stream, num_pixels);
    write_output(output_gray, gray_stream, num_pixels);
}

