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

        outBuf->Value = inBuf->Value + 1;
        bytesReturned = sizeof(MCU_ECHO_INC_DATA);
        status = STATUS_SUCCESS;
        break;
    }

    case IOCTL_MCU_GET_VERSION:
    {
        PMCU_VERSION_OUT outBuf = nullptr;

        if (OutputBufferLength < sizeof(MCU_VERSION_OUT))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        status = WdfRequestRetrieveOutputBuffer(
            Request,
            sizeof(MCU_VERSION_OUT),
            reinterpret_cast<PVOID*>(&outBuf),
            nullptr
        );
        if (!NT_SUCCESS(status))
        {
            break;
        }

        outBuf->Major = 1;
        outBuf->Minor = 0;
        outBuf->Patch = 0;
        outBuf->Reserved = 0;

        bytesReturned = sizeof(MCU_VERSION_OUT);
        status = STATUS_SUCCESS;
        break;
    }

    case IOCTL_MCU_GET_STATUS:
    {
        PMCU_STATUS_OUT outBuf = nullptr;

        if (OutputBufferLength < sizeof(MCU_STATUS_OUT))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        status = WdfRequestRetrieveOutputBuffer(
            Request,
            sizeof(MCU_STATUS_OUT),
            reinterpret_cast<PVOID*>(&outBuf),
            nullptr
        );
        if (!NT_SUCCESS(status))
        {
            break;
        }

        outBuf->DriverReady = 1;
        outBuf->McuConnected = 0;
        outBuf->BusReady = 0;
        outBuf->LastError = 0;

        bytesReturned = sizeof(MCU_STATUS_OUT);
        status = STATUS_SUCCESS;
        break;
    }

    case IOCTL_MCU_SMBUS_READ:
    {
        PMCU_SMBUS_READ_IN inBuf = nullptr;
        PMCU_SMBUS_READ_OUT outBuf = nullptr;
        UINT8 readLen = 0;

        if (InputBufferLength < sizeof(MCU_SMBUS_READ_IN))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        if (OutputBufferLength < sizeof(MCU_SMBUS_READ_OUT))
        {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        status = WdfRequestRetrieveInputBuffer(
            Request,
            sizeof(MCU_SMBUS_READ_IN),
            reinterpret_cast<PVOID*>(&inBuf),
            nullptr
        );
        if (!NT_SUCCESS(status))
        {
            break;
        }

        status = WdfRequestRetrieveOutputBuffer(
            Request,
            sizeof(MCU_SMBUS_READ_OUT),
            reinterpret_cast<PVOID*>(&outBuf),
            nullptr
        );
        if (!NT_SUCCESS(status))
        {
            break;
        }

        if (inBuf->ReadLength == 0 || inBuf->ReadLength > MCU_SMBUS_MAX_DATA)
        {
            status = STATUS_INVALID_PARAMETER;
            break;
        }

        readLen = inBuf->ReadLength;
        if (readLen == 0 || readLen > MCU_SMBUS_MAX_DATA)
        {
            status = STATUS_INVALID_PARAMETER;
            break;
        }

        outBuf->BytesReturned = readLen;

        for (ULONG i = 0; i < readLen; ++i)
        {
            outBuf->Data[i] = (UINT8)(0x10 + i);
        }

        bytesReturned = sizeof(outBuf->BytesReturned) + readLen;
        status = STATUS_SUCCESS;
        break;
    }

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    WdfRequestCompleteWithInformation(Request, status, bytesReturned);
}