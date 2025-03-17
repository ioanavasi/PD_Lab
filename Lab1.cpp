#include <iostream>
#include <windows.h>

int main() {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Services", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        wchar_t subkeyName[256];
        DWORD subkeyNameSize;
        DWORD index = 0;
        while (1) {
            subkeyNameSize = sizeof(subkeyName) / sizeof(subkeyName[0]);
            if (RegEnumKeyExW(hKey, index, subkeyName, &subkeyNameSize, NULL, NULL, NULL, NULL) == ERROR_NO_MORE_ITEMS) {
                break;
            }
            HKEY subkey;
            if (RegOpenKeyExW(hKey, subkeyName, 0, KEY_READ, &subkey) == ERROR_SUCCESS) {
                wchar_t imagePath[MAX_PATH];
                DWORD dataSize = sizeof(imagePath);
                if (RegGetValueW(subkey, NULL, L"ImagePath", RRF_RT_REG_SZ, NULL, imagePath, &dataSize) == ERROR_SUCCESS) {
                    std::wcout << L"Subkey: " << subkeyName << L", ImagePath: " << imagePath << std::endl;
                }
                else {
                    std::wcout << L"Subkey: " << subkeyName << L", ImagePath: Does not exist" << std::endl;
                }
                RegCloseKey(subkey);
            }
            index++;
        }
        RegCloseKey(hKey);
    }
    else {
        std::cerr << "Failed to open registry key." << std::endl;
        return 1;
    }
    return 0;
}
