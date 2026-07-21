#include "image_accel_control.h"

void image_accel_write_reg(uintptr_t base_addr, uint32_t offset, uint32_t value)
{
    volatile uint32_t* reg =
        (volatile uint32_t*)(base_addr + (uintptr_t)offset);
    *reg = value;
}

uint32_t image_accel_read_reg(uintptr_t base_addr, uint32_t offset)
{
    const volatile uint32_t* reg =
        (const volatile uint32_t*)(base_addr + (uintptr_t)offset);
    return *reg;
}

void image_accel_set_input_rgb(uintptr_t base_addr, uint64_t input_rgb_addr)
{
    image_accel_write_reg(base_addr, IMAGE_ACCEL_INPUT_RGB_LOW_OFFSET,
                          (uint32_t)(input_rgb_addr & 0xffffffffu));
    image_accel_write_reg(base_addr, IMAGE_ACCEL_INPUT_RGB_HIGH_OFFSET,
                          (uint32_t)(input_rgb_addr >> 32));
}

void image_accel_set_output_gray(uintptr_t base_addr, uint64_t output_gray_addr)
{
    image_accel_write_reg(base_addr, IMAGE_ACCEL_OUTPUT_LOW_OFFSET,
                          (uint32_t)(output_gray_addr & 0xffffffffu));
    image_accel_write_reg(base_addr, IMAGE_ACCEL_OUTPUT_HIGH_OFFSET,
                          (uint32_t)(output_gray_addr >> 32));
}

void image_accel_set_num_pixels(uintptr_t base_addr, uint32_t num_pixels)
{
    image_accel_write_reg(base_addr, IMAGE_ACCEL_NUM_PIXELS_OFFSET, num_pixels);
}

void image_accel_start(uintptr_t base_addr)
{
    image_accel_write_reg(base_addr, IMAGE_ACCEL_CTRL_OFFSET,
                          IMAGE_ACCEL_CTRL_AP_START);
}

uint32_t image_accel_is_done(uintptr_t base_addr)
{
    return (image_accel_read_reg(base_addr, IMAGE_ACCEL_CTRL_OFFSET) &
            IMAGE_ACCEL_CTRL_AP_DONE) != 0u;
}

uint32_t image_accel_is_idle(uintptr_t base_addr)
{
    return (image_accel_read_reg(base_addr, IMAGE_ACCEL_CTRL_OFFSET) &
            IMAGE_ACCEL_CTRL_AP_IDLE) != 0u;
}

int image_accel_wait_done(uintptr_t base_addr, uint32_t timeout_iterations)
{
    for (uint32_t i = 0; i < timeout_iterations; ++i) {
        if (image_accel_is_done(base_addr)) {
            return 0;
        }
    }

    return -1;
}

int image_accel_run(uintptr_t base_addr,
                    uint64_t input_rgb_addr,
                    uint64_t output_gray_addr,
                    uint32_t num_pixels,
                    uint32_t timeout_iterations)
{
    image_accel_set_input_rgb(base_addr, input_rgb_addr);
    image_accel_set_output_gray(base_addr, output_gray_addr);
    image_accel_set_num_pixels(base_addr, num_pixels);
    image_accel_start(base_addr);

    return image_accel_wait_done(base_addr, timeout_iterations);
}

