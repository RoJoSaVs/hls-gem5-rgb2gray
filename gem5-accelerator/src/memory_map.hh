#include <cstdint>

// Register mapping
static constexpr uint64_t REG_INPUT_ADDR  = 0x00;
static constexpr uint64_t REG_OUTPUT_ADDR = 0x08;
static constexpr uint64_t REG_NUM_PIXELS  = 0x10;
static constexpr uint64_t REG_COMMAND     = 0x14;
static constexpr uint64_t REG_STATUS      = 0x18;

// Accelerator status flags
static constexpr uint32_t CMD_START = 1;
static constexpr uint32_t STATUS_IDLE = 0;
static constexpr uint32_t STATUS_BUSY = 1;
static constexpr uint32_t STATUS_DONE = 2;
static constexpr uint32_t STATUS_ERROR = 3;

// Accelerator constants
static constexpr double ACCEL_NS_PER_PIXEL  = 1.0;   // 1 cycle/px @ 1 GHz
static constexpr uint32_t DMA_CHUNK = 64;
static constexpr uint64_t ACCEL_BASE = 0x10000000ULL;
static constexpr uint64_t ACCEL_SIZE = 256;
