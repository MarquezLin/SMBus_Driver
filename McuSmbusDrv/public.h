#pragma once

#include <ntddk.h>
#include <wdf.h>

#define NTDEVICE_NAME      L"\\Device\\McuSmbusDrv"
#define SYMBOLIC_NAME      L"\\DosDevices\\McuSmbusDrv"

#define IOCTL_MCU_PING CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _MCU_PING_OUT {
    UINT32 Value;
} MCU_PING_OUT, * PMCU_PING_OUT;

EXTERN_C EVT_WDF_DRIVER_DEVICE_ADD McuSmbusDrvEvtDeviceAdd;
EXTERN_C EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL McuSmbusDrvEvtIoDeviceControl;