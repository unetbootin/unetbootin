/* i586-mingw32msvc-gcc runxfile.c -o runxfile.exe -Wl,-subsystem,windows */
/* icl runxfile.c /link Shell32.lib /subsystem:windows */

#include <windows.h>
#include <shellapi.h>

int         argc = 0;
char**      argv = NULL;

int WINAPI WinMain ( HINSTANCE hInst, HINSTANCE hPrev, LPSTR CmdLine, int ShowCmd )
{
SHELLEXECUTEINFO ShExecInfo = {0};
argc = __argc;
argv = __argv;
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
