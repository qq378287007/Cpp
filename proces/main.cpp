#include 

CWorkProcess SartSrv("sc", "start TermService11");

SartSrv.LaunchProcess();  

if (!SartSrv.WaitForEnded(1))
{
 SartSrv.StopProcess();
}