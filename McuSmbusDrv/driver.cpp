#include "public.h"

extern "C" DRIVER_INITIALIZE DriverEntry;

extern "C"
NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    WDF_DRIVER_CONFIG config;
    NTSTATUS status;

    KdPrint(("McuSmbusDrv: DriverEntry\n"));

    WDF_DRIVER_CONFIG_INIT(&config, McuSmbusDrvEvtDeviceAdd);

    status = WdfDriverCreate(
        DriverObject,
        RegistryPath,
        WDF_NO_OBJECT_ATTRIBUTES,
        &config,
        WDF_NO_HANDLE
    );

    if (!NT_SUCCESS(status))
    {
        KdPrint(("McuSmbusDrv: WdfDriverCreate failed\n"));
    }

    return status;
}