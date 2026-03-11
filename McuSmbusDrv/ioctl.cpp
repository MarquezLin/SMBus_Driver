#include "public.h"

VOID
McuSmbusDrvEvtIoDeviceControl(
    _In_ WDFQUEUE Queue,
    _In_ WDFREQUEST Request,
    _In_ size_t OutputBufferLength,
    _In_ size_t InputBufferLength,
    _In_ ULONG IoControlCode
)
{
    UNREFERENCED_PARAMETER(Queue);
    UNREFERENCED_PARAMETER(InputBufferLength);

    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    size_t bytesReturned = 0;

    KdPrint(("McuSmbusDrv: EvtIoDeviceControl, code=0x%08X\n", IoControlCode));

    switch (IoControlCode)
    {
    case IOCTL_MCU_PING:
    {
        PMCU_PING_OUT outBuf = nullptr;

        if (OutputBufferLength < sizeof(MCU_PING_OUT))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        status = WdfRequestRetrieveOutputBuffer(
            Request,
            sizeof(MCU_PING_OUT),
            reinterpret_cast<PVOID*>(&outBuf),
            nullptr
        );

        if (!NT_SUCCESS(status))
        {
            break;
        }

        outBuf->Value = 0x12345678;
        bytesReturned = sizeof(MCU_PING_OUT);
        status = STATUS_SUCCESS;
        break;
    }

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    WdfRequestCompleteWithInformation(Request, status, bytesReturned);
}