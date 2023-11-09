#include "WorkProcess.h"

int main()
{
    CWorkProcess SartSrv("calc", "/h");
    SartSrv.LaunchProcess();
    if (!SartSrv.WaitForEnded(1))
        SartSrv.StopProcess();

    return 0;
}