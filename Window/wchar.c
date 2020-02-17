#include <windows.h>
#include <tchar.h>
#include <stdio.h>

int CDECL MessageBoxPrintf(TCHAR* szCaption, TCHAR* szFormat, ...)
{
    TCHAR szBuffer[1024];
    va_list pArgList;
    // The va_list macro (defined in STDARG.H) is usually equivaient to:
    // pArgList = (char*)&szFormat + sizeof(szFormat)

    va_start(pArgList, szFormat);
    
    // Thre last arguments to wsprintf points to teh arguments

    _vsntprintf(szBuffer, sizeof(szBuffer) / sizeof(TCHAR), szFormat, pArgList);

    // The va_end macro just zeros out pArgList for no good reason
    va_end(pArgList);
    return MessageBox(NULL, szBuffer, szCaption, 0);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    int cxScreen, cyScreen;
    cxScreen = GetSystemMetrics(SM_CXSCREEN);
    cyScreen = GetSystemMetrics(SM_CYSCREEN);

    MessageBoxPrintf(TEXT("ScrnSize"), TEXT("Thre screen is %i pixels wide by %i pixels high")
    , cxScreen, cyScreen);
    return 0;
    
}
