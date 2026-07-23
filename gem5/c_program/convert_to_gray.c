#include <stdint.h>
#include <gem5/m5ops.h>

// Image information
#ifndef IMAGE_WIDTH
#define IMAGE_WIDTH 64U
#endif

#ifndef IMAGE_HEIGHT
#define IMAGE_HEIGHT 64U
#endif
#define NUM_PIXELS        (IMAGE_WIDTH * IMAGE_HEIGHT)
#define RGB_SIZE          (NUM_PIXELS * 3U)
#define GRAY_SIZE         NUM_PIXELS

// Device addresses
#define RAM_RGB_ADDR      0x01000000UL
#define RAM_GRAY_ADDR     0x01800000UL

#define NVM_RGB_ADDR      0x20000000UL
#define NVM_GRAY_ADDR     0x20800000UL

#define ACCEL_BASE        0x10000000UL

// Accelerator information
#define REG_INPUT_ADDR    0x00UL
#define REG_OUTPUT_ADDR   0x08UL
#define REG_NUM_PIXELS    0x10UL
#define REG_COMMAND       0x14UL
#define REG_STATUS        0x18UL

#define CMD_START         1U

#define STATUS_IDLE       0U
#define STATUS_BUSY       1U
#define STATUS_DONE       2U
#define STATUS_ERROR      3U

#define MMIO64(address) \
    (*(volatile uint64_t *)(uintptr_t)(address))

#define MMIO32(address) \
    (*(volatile uint32_t *)(uintptr_t)(address))

static inline void
memory_barrier(void)
{
    __asm__ volatile("dmb sy" ::: "memory");
}

static void
copy_bytes(uintptr_t destination, uintptr_t source, uint32_t length)
{
    volatile uint8_t *dst =
        (volatile uint8_t *)destination;

    volatile const uint8_t *src =
        (volatile const uint8_t *)source;

    for (uint32_t i = 0; i < length; ++i)
        dst[i] = src[i];
}

int
main(void)
{
    /* Copy preloaded RGB image from NVM to RAM. */
    copy_bytes(RAM_RGB_ADDR, NVM_RGB_ADDR, RGB_SIZE);

    memory_barrier();

    /* Configure accelerator with physical RAM addresses. */
    MMIO64(ACCEL_BASE + REG_INPUT_ADDR) = RAM_RGB_ADDR;
    MMIO64(ACCEL_BASE + REG_OUTPUT_ADDR) = RAM_GRAY_ADDR;
    MMIO32(ACCEL_BASE + REG_NUM_PIXELS) = NUM_PIXELS;

    memory_barrier();

    /* Start RGB-to-grayscale conversion. */
    MMIO32(ACCEL_BASE + REG_COMMAND) = CMD_START;

    memory_barrier();

    /* Wait until conversion completes. */
    while (1) {
        const uint32_t status =
            MMIO32(ACCEL_BASE + REG_STATUS);

        if (status == STATUS_DONE)
            break;

        if (status == STATUS_ERROR)
            m5_fail(0, 1);
    }

    memory_barrier();

    /* Copy grayscale result from RAM back into NVM. */
    copy_bytes(NVM_GRAY_ADDR, RAM_GRAY_ADDR, GRAY_SIZE);

    memory_barrier();

    /*
     * Copy the NVM grayscale region to a host file.
     * Use an absolute path to meet the requested output location.
     */
    const char output_file[] = "output.raw";

    const uint64_t written = m5_write_file(
        (void *)NVM_GRAY_ADDR,
        GRAY_SIZE,
        0,
        output_file
    );

    if (written != GRAY_SIZE)
        m5_fail(0, 2);

    m5_exit(0);

    return 0;
}
