#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define IMAGE_ACCEL_MAX_PIXELS 2073600u
#define IMAGE_ACCEL_INPUT_BYTES_PER_PIXEL 3u
#define IMAGE_ACCEL_MAX_RGB_BYTES 6220800u

#define KNOWN_PIXELS 8u
#define PATTERN_PIXELS 1024u

/* The HLS top is implemented in C++, but extern "C" exposes a C ABI. */
extern void image_accel(
    volatile const uint8_t* input_rgb,
    volatile uint8_t* output_gray,
    uint32_t num_pixels);

/*
 * Co-simulation models the full AXI pointer depths, not only num_pixels.
 * Keep these static buffers at the declared interface depths so the wrapper
 * can safely inspect the complete modeled memory range.
 */
static uint8_t g_input_rgb[IMAGE_ACCEL_MAX_RGB_BYTES];
static uint8_t g_output_gray[IMAGE_ACCEL_MAX_PIXELS];

static uint8_t golden_gray(uint8_t r, uint8_t g, uint8_t b)
{
    return (uint8_t)((77u * r + 150u * g + 29u * b) >> 8);
}

static void set_rgb(uint8_t* input_rgb, uint32_t pixel_index,
                    uint8_t r, uint8_t g, uint8_t b)
{
    const uint32_t base = pixel_index * IMAGE_ACCEL_INPUT_BYTES_PER_PIXEL;

    input_rgb[base] = r;
    input_rgb[base + 1u] = g;
    input_rgb[base + 2u] = b;
}

static bool check_output(const uint8_t* input_rgb, const uint8_t* output_gray,
                         uint32_t num_pixels, const char* test_name)
{
    bool pass = true;
    uint32_t i;

    for (i = 0; i < num_pixels; ++i) {
        const uint32_t base = i * IMAGE_ACCEL_INPUT_BYTES_PER_PIXEL;
        const uint8_t expected =
            golden_gray(input_rgb[base], input_rgb[base + 1u],
                        input_rgb[base + 2u]);

        if (output_gray[i] != expected) {
            fprintf(stderr,
                    "FAIL [%s] pixel %u: RGB=(%u, %u, %u) expected %u got %u\n",
                    test_name, (unsigned int)i, (unsigned int)input_rgb[base],
                    (unsigned int)input_rgb[base + 1u],
                    (unsigned int)input_rgb[base + 2u],
                    (unsigned int)expected, (unsigned int)output_gray[i]);
            pass = false;
        }
    }

    return pass;
}

static bool run_known_pixel_test(void)
{
    uint32_t i;

    for (i = 0; i < KNOWN_PIXELS; ++i) {
        g_output_gray[i] = 0xA5u;
    }

    set_rgb(g_input_rgb, 0u, 0u, 0u, 0u);
    set_rgb(g_input_rgb, 1u, 255u, 255u, 255u);
    set_rgb(g_input_rgb, 2u, 255u, 0u, 0u);
    set_rgb(g_input_rgb, 3u, 0u, 255u, 0u);
    set_rgb(g_input_rgb, 4u, 0u, 0u, 255u);
    set_rgb(g_input_rgb, 5u, 16u, 32u, 64u);
    set_rgb(g_input_rgb, 6u, 123u, 45u, 67u);
    set_rgb(g_input_rgb, 7u, 200u, 180u, 40u);

    image_accel(g_input_rgb, g_output_gray, KNOWN_PIXELS);

    return check_output(g_input_rgb, g_output_gray, KNOWN_PIXELS,
                        "known pixels");
}

static bool run_pattern_test(void)
{
    uint32_t i;

    for (i = 0; i < PATTERN_PIXELS; ++i) {
        g_output_gray[i] = 0x5Au;
        set_rgb(g_input_rgb, i, (uint8_t)((3u * i + 17u) & 0xFFu),
                (uint8_t)((5u * i + 29u) & 0xFFu),
                (uint8_t)((7u * i + 43u) & 0xFFu));
    }

    image_accel(g_input_rgb, g_output_gray, PATTERN_PIXELS);

    return check_output(g_input_rgb, g_output_gray, PATTERN_PIXELS,
                        "pattern");
}

int main(void)
{
    const bool known_pass = run_known_pixel_test();
    const bool pattern_pass = run_pattern_test();

    if (!known_pass || !pattern_pass) {
        fprintf(stderr, "image_accel testbench failed\n");
        return 1;
    }

    printf("image_accel testbench passed\n");
    return 0;
}
