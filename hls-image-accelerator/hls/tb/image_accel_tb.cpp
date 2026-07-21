#include "../src/image_accel.hpp"

#include <cstdint>
#include <iostream>

namespace {

constexpr uint32_t kKnownPixels = 8;
constexpr uint32_t kPatternPixels = 1024;

// The co-simulation wrapper dumps memory according to the m_axi interface
// depths, not only num_pixels. Keep full-depth static buffers so wrapc can
// safely read/write the complete modeled AXI memory range.
static uint8_t g_input_rgb[IMAGE_ACCEL_MAX_RGB_BYTES];
static uint8_t g_output_gray[IMAGE_ACCEL_MAX_PIXELS];

uint8_t golden_gray(uint8_t r, uint8_t g, uint8_t b)
{
    return static_cast<uint8_t>((77u * r + 150u * g + 29u * b) >> 8);
}

void set_rgb(uint8_t* input_rgb, uint32_t pixel_index,
             uint8_t r, uint8_t g, uint8_t b)
{
    const uint32_t base = pixel_index * IMAGE_ACCEL_INPUT_BYTES_PER_PIXEL;
    input_rgb[base] = r;
    input_rgb[base + 1u] = g;
    input_rgb[base + 2u] = b;
}

bool check_output(const uint8_t* input_rgb, const uint8_t* output_gray,
                  uint32_t num_pixels, const char* test_name)
{
    bool pass = true;

    for (uint32_t i = 0; i < num_pixels; ++i) {
        const uint32_t base = i * IMAGE_ACCEL_INPUT_BYTES_PER_PIXEL;
        const uint8_t expected =
            golden_gray(input_rgb[base], input_rgb[base + 1u],
                        input_rgb[base + 2u]);

        if (output_gray[i] != expected) {
            std::cerr << "FAIL [" << test_name << "] pixel " << i
                      << ": RGB=(" << static_cast<unsigned>(input_rgb[base])
                      << ", " << static_cast<unsigned>(input_rgb[base + 1u])
                      << ", " << static_cast<unsigned>(input_rgb[base + 2u])
                      << ") expected " << static_cast<unsigned>(expected)
                      << " got " << static_cast<unsigned>(output_gray[i])
                      << '\n';
            pass = false;
        }
    }

    return pass;
}

bool run_known_pixel_test()
{
    for (uint32_t i = 0; i < kKnownPixels; ++i) {
        g_output_gray[i] = 0xA5;
    }

    set_rgb(g_input_rgb, 0, 0, 0, 0);
    set_rgb(g_input_rgb, 1, 255, 255, 255);
    set_rgb(g_input_rgb, 2, 255, 0, 0);
    set_rgb(g_input_rgb, 3, 0, 255, 0);
    set_rgb(g_input_rgb, 4, 0, 0, 255);
    set_rgb(g_input_rgb, 5, 16, 32, 64);
    set_rgb(g_input_rgb, 6, 123, 45, 67);
    set_rgb(g_input_rgb, 7, 200, 180, 40);

    image_accel(g_input_rgb, g_output_gray, kKnownPixels);

    return check_output(g_input_rgb, g_output_gray, kKnownPixels,
                        "known pixels");
}

bool run_pattern_test()
{
    for (uint32_t i = 0; i < kPatternPixels; ++i) {
        g_output_gray[i] = 0x5A;
    }

    for (uint32_t i = 0; i < kPatternPixels; ++i) {
        const uint8_t r = static_cast<uint8_t>((3u * i + 17u) & 0xFFu);
        const uint8_t g = static_cast<uint8_t>((5u * i + 29u) & 0xFFu);
        const uint8_t b = static_cast<uint8_t>((7u * i + 43u) & 0xFFu);
        set_rgb(g_input_rgb, i, r, g, b);
    }

    image_accel(g_input_rgb, g_output_gray, kPatternPixels);

    return check_output(g_input_rgb, g_output_gray, kPatternPixels,
                        "pattern");
}

} // namespace

int main()
{
    const bool known_pass = run_known_pixel_test();
    const bool pattern_pass = run_pattern_test();

    if (!known_pass || !pattern_pass) {
        std::cerr << "image_accel testbench failed\n";
        return 1;
    }

    std::cout << "image_accel testbench passed\n";
    return 0;
}
