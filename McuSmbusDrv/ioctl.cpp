#include "public.h"
#include <wdmsec.h>

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

    case IOCTL_MCU_ECHO_INC:
    {
        
        PMCU_ECHO_INC_DATA inBuf = nullptr;
        PMCU_ECHO_INC_DATA outBuf = nullptr;

        KdPrint(("ECHO_INC VERSION\n"));

        if (InputBufferLength < sizeof(MCU_ECHO_INC_DATA) ||
            OutputBufferLength < sizeof(MCU_ECHO_INC_DATA))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        status = WdfRequestRetrieveInputBuffer(
            Request,
            sizeof(MCU_ECHO_INC_DATA),
            reinterpret_cast<PVOID*>(&inBuf),
            nullptr
        );

        if (!NT_SUCCESS(status))
        {
            break;
        }

        status = WdfRequestRetrieveOutputBuffer(
            Request,
            sizeof(MCU_ECHO_INC_DATA),
            reinterpret_cast<PVOID*>(&outBuf),
            nullptr
        );

        if (!NT_SUCCESS(status))
        {
            break;
        }

        KdPrint(("ECHO_INC: in=0x%08X\n", inBuf->Value));

        outBuf->Value = inBuf->Value + 1;

        KdPrint(("ECHO_INC: out=0x%08X\n", outBuf->Value));

        bytesReturned = sizeof(MCU_ECHO_INC_DATA);
        status = STATUS_SUCCESS;
        break;
    }

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    WdfRequestCompleteWithInformation(Request, status, bytesReturned);
}