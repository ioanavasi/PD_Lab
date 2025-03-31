#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <initguid.h>
#include <cfgmgr32.h>
#include <tchar.h>
#include <iostream>
#include <iomanip>

#pragma comment(lib, "setupapi.lib")

void ListUSBDevices()
{
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    DWORD i;

    // Create a device information set for all USB devices
    hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_USB, NULL, NULL, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Error getting device information set: " << GetLastError() << std::endl;
        return;
    }

    // Enumerate through all devices in the set
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
    {
        DWORD DataT;
        LPTSTR buffer = NULL;
        DWORD buffersize = 0;

        // Get the device description
        while (!SetupDiGetDeviceRegistryProperty(
            hDevInfo,
            &DeviceInfoData,
            SPDRP_DEVICEDESC,
            &DataT,
            (PBYTE)buffer,
            buffersize,
            &buffersize))
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (buffer) LocalFree(buffer);
                buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
            }
            else
            {
                std::cerr << "Error getting device description: " << GetLastError() << std::endl;
                break;
            }
        }

        if (buffer)
        {
            std::wcout << L"Device: " << buffer << std::endl;
            LocalFree(buffer);
        }

        // Get the hardware ID
        buffersize = 0;
        buffer = NULL;
        if (SetupDiGetDeviceRegistryProperty(
            hDevInfo,
            &DeviceInfoData,
            SPDRP_HARDWAREID,
            &DataT,
            NULL,
            0,
            &buffersize))
        {
            buffer = (LPTSTR)LocalAlloc(LPTR, buffersize);
            if (SetupDiGetDeviceRegistryProperty(
                hDevInfo,
                &DeviceInfoData,
                SPDRP_HARDWAREID,
                &DataT,
                (PBYTE)buffer,
                buffersize,
                &buffersize))
            {
                std::wcout << L"Hardware ID: " << buffer << std::endl;
            }
            LocalFree(buffer);
        }

        // Get the manufacturer
        buffersize = 0;
        buffer = NULL;
        if (SetupDiGetDeviceRegistryProperty(
            hDevInfo,
            &DeviceInfoData,
            SPDRP_MFG,
            &DataT,
            NULL,
            0,
            &buffersize))
        {
            buffer = (LPTSTR)LocalAlloc(LPTR, buffersize);
            if (SetupDiGetDeviceRegistryProperty(
                hDevInfo,
                &DeviceInfoData,
                SPDRP_MFG,
                &DataT,
                (PBYTE)buffer,
                buffersize,
                &buffersize))
            {
                std::wcout << L"Manufacturer: " << buffer << std::endl;
            }
            LocalFree(buffer);
        }

        // Get the device instance ID
        TCHAR instanceID[MAX_DEVICE_ID_LEN];
        if (CM_Get_Device_ID(DeviceInfoData.DevInst, instanceID, MAX_DEVICE_ID_LEN, 0) == CR_SUCCESS)
        {
            std::wcout << L"Instance ID: " << instanceID << std::endl;
        }

        std::wcout << std::endl;
    }

    if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS)
    {
        std::cerr << "Error enumerating devices: " << GetLastError() << std::endl;
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
}

int main()
{
    std::wcout << L"Listing all connected USB devices:" << std::endl;
    std::wcout << L"=================================" << std::endl << std::endl;
    
    ListUSBDevices();
    
    std::wcout << L"Enumeration complete." << std::endl;
    return 0;
}