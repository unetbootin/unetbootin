#include <windows.h>

int main (int argc, char **argv)
{
SHELLEXECUTEINFO ShExecInfo = {0};
ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
ShExecInfo.hwnd = NULL;
ShExecInfo.lpVerb = "runas";
ShExecInfo.lpFile = argv[1];
ShExecInfo.lpParameters = "";	
ShExecInfo.lpDirectory = NULL;
ShExecInfo.nShow = SW_SHOW;
ShExecInfo.hInstApp = NULL;
ShellExecuteEx(&ShExecInfo);
WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
return 0;
}
