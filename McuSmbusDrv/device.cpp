#include "public.h"

NTSTATUS
McuSmbusDrvEvtDeviceAdd(
    _In_ WDFDRIVER Driver,
    _Inout_ PWDFDEVICE_INIT DeviceInit
)
{
    UNREFERENCED_PARAMETER(Driver);

    NTSTATUS status;
    WDFDEVICE device;
    WDF_IO_QUEUE_CONFIG queueConfig;
    UNICODE_STRING devName;
    UNICODE_STRING symLink;

    KdPrint(("McuSmbusDrv: EvtDeviceAdd\n"));

    RtlInitUnicodeString(&devName, NTDEVICE_NAME);
    status = WdfDeviceInitAssignName(DeviceInit, &devName);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("McuSmbusDrv: WdfDeviceInitAssignName failed = 0x%08X\n", status));
        return status;
    }

    status = WdfDeviceCreate(&DeviceInit, WDF_NO_OBJECT_ATTRIBUTES, &device);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("McuSmbusDrv: WdfDeviceCreate failed = 0x%08X\n", status));
        return status;
    }

    RtlInitUnicodeString(&symLink, SYMBOLIC_NAME);
    status = WdfDeviceCreateSymbolicLink(device, &symLink);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("McuSmbusDrv: WdfDeviceCreateSymbolicLink failed = 0x%08X\n", status));
        return status;
    }

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchSequential);
    queueConfig.EvtIoDeviceControl = McuSmbusDrvEvtIoDeviceControl;

    status = WdfIoQueueCreate(
        device,
        &queueConfig,
        WDF_NO_OBJECT_ATTRIBUTES,
        WDF_NO_HANDLE
    );

    if (!NT_SUCCESS(status))
    {
        KdPrint(("McuSmbusDrv: WdfIoQueueCreate failed = 0x%08X\n", status));
        return status;
    }

    KdPrint(("McuSmbusDrv: Device created successfully\n"));
    return STATUS_SUCCESS;
}