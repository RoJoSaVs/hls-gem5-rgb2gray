// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2024.1 (64-bit)
// Tool Version Limit: 2024.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
// control
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read/COR)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read/COR)
//        bit 7  - auto_restart (Read/Write)
//        bit 9  - interrupt (Read)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0 - enable ap_done interrupt (Read/Write)
//        bit 1 - enable ap_ready interrupt (Read/Write)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0 - ap_done (Read/TOW)
//        bit 1 - ap_ready (Read/TOW)
//        others - reserved
// 0x10 : Data signal of input_rgb
//        bit 31~0 - input_rgb[31:0] (Read/Write)
// 0x14 : Data signal of input_rgb
//        bit 31~0 - input_rgb[63:32] (Read/Write)
// 0x18 : reserved
// 0x1c : Data signal of output_gray
//        bit 31~0 - output_gray[31:0] (Read/Write)
// 0x20 : Data signal of output_gray
//        bit 31~0 - output_gray[63:32] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of num_pixels
//        bit 31~0 - num_pixels[31:0] (Read/Write)
// 0x2c : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XIMAGE_ACCEL_CONTROL_ADDR_AP_CTRL          0x00
#define XIMAGE_ACCEL_CONTROL_ADDR_GIE              0x04
#define XIMAGE_ACCEL_CONTROL_ADDR_IER              0x08
#define XIMAGE_ACCEL_CONTROL_ADDR_ISR              0x0c
#define XIMAGE_ACCEL_CONTROL_ADDR_INPUT_RGB_DATA   0x10
#define XIMAGE_ACCEL_CONTROL_BITS_INPUT_RGB_DATA   64
#define XIMAGE_ACCEL_CONTROL_ADDR_OUTPUT_GRAY_DATA 0x1c
#define XIMAGE_ACCEL_CONTROL_BITS_OUTPUT_GRAY_DATA 64
#define XIMAGE_ACCEL_CONTROL_ADDR_NUM_PIXELS_DATA  0x28
#define XIMAGE_ACCEL_CONTROL_BITS_NUM_PIXELS_DATA  32

