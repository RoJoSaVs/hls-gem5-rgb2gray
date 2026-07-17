// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2024.1 (64-bit)
// Tool Version Limit: 2024.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef XIMAGE_ACCEL_H
#define XIMAGE_ACCEL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "ximage_accel_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
#ifdef SDT
    char *Name;
#else
    u16 DeviceId;
#endif
    u64 Control_BaseAddress;
} XImage_accel_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XImage_accel;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XImage_accel_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XImage_accel_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XImage_accel_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XImage_accel_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
#ifdef SDT
int XImage_accel_Initialize(XImage_accel *InstancePtr, UINTPTR BaseAddress);
XImage_accel_Config* XImage_accel_LookupConfig(UINTPTR BaseAddress);
#else
int XImage_accel_Initialize(XImage_accel *InstancePtr, u16 DeviceId);
XImage_accel_Config* XImage_accel_LookupConfig(u16 DeviceId);
#endif
int XImage_accel_CfgInitialize(XImage_accel *InstancePtr, XImage_accel_Config *ConfigPtr);
#else
int XImage_accel_Initialize(XImage_accel *InstancePtr, const char* InstanceName);
int XImage_accel_Release(XImage_accel *InstancePtr);
#endif

void XImage_accel_Start(XImage_accel *InstancePtr);
u32 XImage_accel_IsDone(XImage_accel *InstancePtr);
u32 XImage_accel_IsIdle(XImage_accel *InstancePtr);
u32 XImage_accel_IsReady(XImage_accel *InstancePtr);
void XImage_accel_EnableAutoRestart(XImage_accel *InstancePtr);
void XImage_accel_DisableAutoRestart(XImage_accel *InstancePtr);

void XImage_accel_Set_input_rgb(XImage_accel *InstancePtr, u64 Data);
u64 XImage_accel_Get_input_rgb(XImage_accel *InstancePtr);
void XImage_accel_Set_output_gray(XImage_accel *InstancePtr, u64 Data);
u64 XImage_accel_Get_output_gray(XImage_accel *InstancePtr);
void XImage_accel_Set_num_pixels(XImage_accel *InstancePtr, u32 Data);
u32 XImage_accel_Get_num_pixels(XImage_accel *InstancePtr);

void XImage_accel_InterruptGlobalEnable(XImage_accel *InstancePtr);
void XImage_accel_InterruptGlobalDisable(XImage_accel *InstancePtr);
void XImage_accel_InterruptEnable(XImage_accel *InstancePtr, u32 Mask);
void XImage_accel_InterruptDisable(XImage_accel *InstancePtr, u32 Mask);
void XImage_accel_InterruptClear(XImage_accel *InstancePtr, u32 Mask);
u32 XImage_accel_InterruptGetEnabled(XImage_accel *InstancePtr);
u32 XImage_accel_InterruptGetStatus(XImage_accel *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
