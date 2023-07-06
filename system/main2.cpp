
#include <windows.h>
#include <stdio.h>

int main(int argc, PCHAR argv[])
{
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
        printf("处理器架构: X64(AMD or Intel)\n");
    else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM)
        printf("处理器架构: AMD\n");
    else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
        printf("处理器架构: Intel 服务器\n");
    else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        printf("处理器架构: X86\n");
    else
        printf("处理器架构: 未知的架构\n");

    printf("程序或DLL最低可用内存地址: %lX\n", si.lpMinimumApplicationAddress);
    printf("程序或DLL最高可用内存地址: %lX\n", si.lpMaximumApplicationAddress);
    printf("配置到系统的处理器: %ld 15 = 0000 0000 0000 0000 0000 0000 0000 1111\n", long(si.dwActiveProcessorMask));

    /// 常用于获得当前可处理数据的子线程
    printf("当前组中的逻辑处理器个数: %d 个\n", si.dwNumberOfProcessors);
    printf("页面分配粒度: 0x%x 字节\n", si.dwPageSize);
    printf("虚拟内存分配粒度: 0x%x 字节\n", si.dwAllocationGranularity);

    if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_INTEL)
        printf("依赖架构的处理器级别: %d [仅用于显示][由CPU供应商定义]\n", si.wProcessorLevel);
    else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
        printf("依赖架构的处理器级别: %d [仅用于显示][设置为1]\n", si.wProcessorLevel);

    printf("处理器的型号: %X\n", si.wProcessorRevision);
    return 0;
}