#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <setupapi.h>
#include <hidsdi.h>

int main() {
    unsigned int input_val;
    printf("Enter a number to send to Blue Pill: ");
    if (scanf("%u", &input_val) != 1) {
        printf("Invalid input.\n");
        return 1;
    }

    unsigned char target_value = (unsigned char)input_val;

    GUID hidGuid;
    HidD_GetHidGuid(&hidGuid);

    HDEVINFO deviceInfoTable = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    if (deviceInfoTable == INVALID_HANDLE_VALUE) {
        printf("Error: Could not get device list.\n");
        return 1;
    }

    SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
    deviceInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    DWORD memberIndex = 0;
    HANDLE hDevice = INVALID_HANDLE_VALUE;

    // مقادیر VID و PID دستگاه بلوپیل خود را اینجا وارد کنید
    USHORT targetVID = 0x0483;
    USHORT targetPID = 0x5710;

    while (SetupDiEnumDeviceInterfaces(deviceInfoTable, NULL, &hidGuid, memberIndex, &deviceInterfaceData)) {
        memberIndex++;

        DWORD requiredSize = 0;
        SetupDiGetDeviceInterfaceDetail(deviceInfoTable, &deviceInterfaceData, NULL, 0, &requiredSize, NULL);

        PSP_DEVICE_INTERFACE_DETAIL_DATA deviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
        deviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (SetupDiGetDeviceInterfaceDetail(deviceInfoTable, &deviceInterfaceData, deviceInterfaceDetailData, requiredSize, NULL, NULL)) {
            HANDLE hTest = CreateFile(deviceInterfaceDetailData->DevicePath, GENERIC_READ | GENERIC_WRITE,
                                      FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);

            if (hTest != INVALID_HANDLE_VALUE) {
                HIDD_ATTRIBUTES attrib;
                attrib.Size = sizeof(HIDD_ATTRIBUTES);
                if (HidD_GetAttributes(hTest, &attrib)) {
                    if (attrib.VendorID == targetVID && attrib.ProductID == targetPID) {
                        hDevice = hTest;
                        free(deviceInterfaceDetailData);
                        break;
                    }
                }
                CloseHandle(hTest);
            }
        }
        free(deviceInterfaceDetailData);
    }

    SetupDiDestroyDeviceInfoList(deviceInfoTable);

    if (hDevice == INVALID_HANDLE_VALUE) {
        printf("Error: Blue Pill HID device not found.\n");
        return 1;
    }

    // ساخت پکت ارسال (بایت اول Report ID و بایت دوم عدد کاربر)
    unsigned char buffer[65] = {0}; 
    buffer[0] = 0;             
    buffer[1] = target_value;  

    if (HidD_SetOutputReport(hDevice, buffer, sizeof(buffer))) {
        printf("Successfully sent number %u to Blue Pill.\n", target_value);
    } else {
        printf("Error: Failed to send data.\n");
    }

    CloseHandle(hDevice);
    return 0;
