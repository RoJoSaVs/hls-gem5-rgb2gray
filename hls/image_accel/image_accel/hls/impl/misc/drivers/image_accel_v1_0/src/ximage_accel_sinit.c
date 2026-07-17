// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2024.1 (64-bit)
// Tool Version Limit: 2024.05
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// Copyright 2022-2024 Advanced Micro Devices, Inc. All Rights Reserved.
// 
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#ifdef SDT
#include "xparameters.h"
#endif
#include "ximage_accel.h"

extern XImage_accel_Config XImage_accel_ConfigTable[];

#ifdef SDT
XImage_accel_Config *XImage_accel_LookupConfig(UINTPTR BaseAddress) {
	XImage_accel_Config *ConfigPtr = NULL;

	int Index;

	for (Index = (u32)0x0; XImage_accel_ConfigTable[Index].Name != NULL; Index++) {
		if (!BaseAddress || XImage_accel_ConfigTable[Index].Control_BaseAddress == BaseAddress) {
			ConfigPtr = &XImage_accel_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XImage_accel_Initialize(XImage_accel *InstancePtr, UINTPTR BaseAddress) {
	XImage_accel_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XImage_accel_LookupConfig(BaseAddress);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XImage_accel_CfgInitialize(InstancePtr, ConfigPtr);
}
#else
XImage_accel_Config *XImage_accel_LookupConfig(u16 DeviceId) {
	XImage_accel_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XIMAGE_ACCEL_NUM_INSTANCES; Index++) {
		if (XImage_accel_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XImage_accel_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XImage_accel_Initialize(XImage_accel *InstancePtr, u16 DeviceId) {
	XImage_accel_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XImage_accel_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XImage_accel_CfgInitialize(InstancePtr, ConfigPtr);
}
#endif

#endif

