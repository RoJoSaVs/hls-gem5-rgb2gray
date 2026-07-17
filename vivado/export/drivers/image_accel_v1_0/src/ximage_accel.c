// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2024.1 (64-bit)
// Tool Version Limit: 2024.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
/***************************** Include Files *********************************/
#include "ximage_accel.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XImage_accel_CfgInitialize(XImage_accel *InstancePtr, XImage_accel_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XImage_accel_Start(XImage_accel *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_AP_CTRL) & 0x80;
    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XImage_accel_IsDone(XImage_accel *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XImage_accel_IsIdle(XImage_accel *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XImage_accel_IsReady(XImage_accel *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XImage_accel_EnableAutoRestart(XImage_accel *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XImage_accel_DisableAutoRestart(XImage_accel *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_AP_CTRL, 0);
}

void XImage_accel_Set_input_rgb(XImage_accel *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_INPUT_RGB_DATA, (u32)(Data));
    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_INPUT_RGB_DATA + 4, (u32)(Data >> 32));
}

u64 XImage_accel_Get_input_rgb(XImage_accel *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_INPUT_RGB_DATA);
    Data += (u64)XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_INPUT_RGB_DATA + 4) << 32;
    return Data;
}

void XImage_accel_Set_output_gray(XImage_accel *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_OUTPUT_GRAY_DATA, (u32)(Data));
    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_OUTPUT_GRAY_DATA + 4, (u32)(Data >> 32));
}

u64 XImage_accel_Get_output_gray(XImage_accel *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_OUTPUT_GRAY_DATA);
    Data += (u64)XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_OUTPUT_GRAY_DATA + 4) << 32;
    return Data;
}

void XImage_accel_Set_num_pixels(XImage_accel *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_NUM_PIXELS_DATA, Data);
}

u32 XImage_accel_Get_num_pixels(XImage_accel *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_NUM_PIXELS_DATA);
    return Data;
}

void XImage_accel_InterruptGlobalEnable(XImage_accel *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_GIE, 1);
}

void XImage_accel_InterruptGlobalDisable(XImage_accel *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_GIE, 0);
}

void XImage_accel_InterruptEnable(XImage_accel *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_IER);
    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_IER, Register | Mask);
}

void XImage_accel_InterruptDisable(XImage_accel *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_IER);
    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_IER, Register & (~Mask));
}

void XImage_accel_InterruptClear(XImage_accel *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XImage_accel_WriteReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_ISR, Mask);
}

u32 XImage_accel_InterruptGetEnabled(XImage_accel *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_IER);
}

u32 XImage_accel_InterruptGetStatus(XImage_accel *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XImage_accel_ReadReg(InstancePtr->Control_BaseAddress, XIMAGE_ACCEL_CONTROL_ADDR_ISR);
}

