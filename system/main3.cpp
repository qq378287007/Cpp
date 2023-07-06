#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <iphlpapi.h>

#include <iostream>

int main()
{
    // 获取操作系统版本信息
    // OSVERSIONINFOEX osvi = {0};
    // osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    OSVERSIONINFOEX osvi = {sizeof(OSVERSIONINFOEX)};
    GetVersionEx((LPOSVERSIONINFO)&osvi);
    std::cout << "Operating System: Windows " << osvi.dwMajorVersion << "." << osvi.dwMinorVersion << "." << osvi.dwBuildNumber << std::endl;

    // 获取处理器信息
    SYSTEM_INFO sysinfo = {0};
    GetSystemInfo(&sysinfo);
    std::cout << "Processor: " << sysinfo.dwNumberOfProcessors << " x " << sysinfo.dwProcessorType << " MHz" << std::endl;

    // 获取内存信息
    // MEMORYSTATUSEX meminfo = {0};
    // meminfo.dwLength = sizeof(MEMORYSTATUSEX);
    MEMORYSTATUSEX meminfo = {sizeof(MEMORYSTATUSEX)};
    GlobalMemoryStatusEx(&meminfo);
    std::cout << "Memory: " << meminfo.ullTotalPhys / (1024 * 1024) << " MB" << std::endl;

    // 获取硬盘信息
    ULARGE_INTEGER diskinfo = {0};
    GetDiskFreeSpaceEx(_T("C:\\"), NULL, &diskinfo, NULL);
    std::cout << "Disk: " << diskinfo.QuadPart / (1024 * 1024 * 1024) << " GB" << std::endl;

    // 获取显示器信息
    HDC hdc = GetDC(NULL);
    int width = GetDeviceCaps(hdc, HORZRES);
    int height = GetDeviceCaps(hdc, VERTRES);
    int bpp = GetDeviceCaps(hdc, BITSPIXEL);
    ReleaseDC(NULL, hdc);
    std::cout << "Display: " << width << " x " << height << " x " << bpp << " bits" << std::endl;

    // 获取网络信息
    DWORD buflen = 0;
    GetAdaptersInfo(NULL, &buflen);

    IP_ADAPTER_INFO *adapterinfo = (IP_ADAPTER_INFO *)malloc(buflen);
    GetAdaptersInfo(adapterinfo, &buflen);
    std::cout << "Network: " << adapterinfo->IpAddressList.IpAddress.String << std::endl;
    free(adapterinfo);

    // 获取驱动程序信息
    TCHAR filename[MAX_PATH] = {0};
    GetModuleFileName(NULL, filename, MAX_PATH);
    DWORD handle = 0;
    DWORD size = GetFileVersionInfoSize(filename, &handle);
    if (size > 0)
    {
        BYTE *buffer = new BYTE[size];
        if (GetFileVersionInfo(filename, handle, size, buffer))
        {
            VS_FIXEDFILEINFO *fileInfo = NULL;
            UINT len = 0;
            if (VerQueryValue(buffer, _T("\\"), (void **)&fileInfo, &len))
                std::cout << "Driver Version: " << HIWORD(fileInfo->dwProductVersionMS) << "." << LOWORD(fileInfo->dwProductVersionMS) << "." << HIWORD(fileInfo->dwProductVersionLS) << "." << LOWORD(fileInfo->dwProductVersionLS) << std::endl;
        }
        delete[] buffer;
    }

    // 电源信息
    SYSTEM_POWER_STATUS powerStatus;
    if (GetSystemPowerStatus(&powerStatus))
    {
        std::cout << "AC Line Status: " << static_cast<int>(powerStatus.ACLineStatus) << std::endl;
        std::cout << "Battery Flag: " << static_cast<int>(powerStatus.BatteryFlag) << std::endl;
        std::cout << "Battery Life Percent: " << static_cast<int>(powerStatus.BatteryLifePercent) << std::endl;
        std::cout << "Battery Life Time: " << powerStatus.BatteryLifeTime << std::endl;
        std::cout << "Battery Full Life Time: " << powerStatus.BatteryFullLifeTime << std::endl;
    }
    return 0;
}

// g++ main3.cpp -o main3 -lGdi32 -liphlpapi -lversion && main3.exe
