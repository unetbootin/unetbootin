/* i586-mingw32msvc-gcc runxfile.c -o runxfile.exe -Wl,-subsystem,windows */

#include <windows.h>

int main (int argc, char **argv)
{
SHELLEXECUTEINFO ShExecInfo = {0};
ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
ShExecInfo.hwnd = NULL;
ShExecInfo.lpVerb = argv[2];
ShExecInfo.lpFile = argv[1];
ShExecInfo.lpParameters = "";	
ShExecInfo.lpDirectory = NULL;
ShExecInfo.nShow = SW_HIDE;
ShExecInfo.hInstApp = NULL;
ShellExecuteEx(&ShExecInfo);
WaitForSingleObject(ShExecInfo.hProcess,INFINITE);
return 0;
}
