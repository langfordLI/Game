#include <windows.h>
#include <stdbool.h>
#include <math.h>

#define NUMLINES ((int)(sizeof devcaps / sizeof devcaps[0]))
void DrawBezier(HDC hdc, POINT apt[]);
void Show(HWND hwnd, HDC hdc, int xText, int yText, int iMapMode, char *szMapMode);
/*
in wingdi.h
*/
struct
{
    int iIndex;
    TCHAR *szLabel;
    TCHAR *szDesc;
} devcaps[] =
    {
        HORZSIZE, TEXT("HORZSIZE"), TEXT("Width in millimeters:"),
        VERTSIZE, TEXT("VERTSIZE"), TEXT("Height in millimeters:"),
        HORZRES, TEXT("HORZRES"), TEXT("Width in pixels:"),
        VERTRES, TEXT("VERTRES"), TEXT("Height in raster lines:"),
        BITSPIXEL, TEXT("BITSPIXEL"), TEXT("Color bits per pixel:"),
        PLANES, TEXT("PLANES"), TEXT("Number of color planes:"),
        NUMBRUSHES, TEXT("NUMBRUSHES"), TEXT("Number of device brushes:"),
        NUMPENS, TEXT("NUMPENS"), TEXT("Number of device pens:"),
        NUMMARKERS, TEXT("NUMMARKERS"), TEXT("Number of device markers:"),
        NUMFONTS, TEXT("NUMFONTS"), TEXT("Number of device fonts:"),
        NUMCOLORS, TEXT("NUMCOLORS"), TEXT("Number of device colors:"),
        PDEVICESIZE, TEXT("PDEVICESIZE"), TEXT("Size of device structure:"),
        ASPECTX, TEXT("ASPECTX"), TEXT("Relative width of pixel:"),
        ASPECTY, TEXT("ASPECTY"), TEXT("Relative height of pixel:"),
        ASPECTXY, TEXT("ASPECTXY"), TEXT("Relative diagonal of pixel:"),
        LOGPIXELSX, TEXT("LOGPIXELSX"), TEXT("Horizontal dots per inch:"),
        LOGPIXELSY, TEXT("LOGPIXELSY"), TEXT("Vertical dots per inch:"),
        SIZEPALETTE, TEXT("SIZEPALETTE"), TEXT("Number of palette entries:"),
        NUMRESERVED, TEXT("NUMRESERVED"), TEXT("Reserved palette entries:"),
        COLORRES, TEXT("COLORRES"), TEXT("Actual color resolution:")};

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static char szAppName[] = "DevCaps";
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    //set the new window's properties
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WinProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = szAppName;
    return RegisterClass(&wc);
}

bool InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    //create a new window
    HWND hWnd = CreateWindow(
        szAppName,                    //window class
        szAppName,                    //title bar
        WS_OVERLAPPEDWINDOW,          //window style
        CW_USEDEFAULT, CW_USEDEFAULT, //position of window
        CW_USEDEFAULT, CW_USEDEFAULT, //dimensions of the window
        NULL,                         //parent window (not used)
        NULL,                         //menu (not used)
        hInstance,                    //application instance
        NULL);                        //window parameters (not used)

    //was there an error creating the window?
    if (hWnd == 0)
        return 0;

    //display the window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreIstance, PSTR szCmdLine, int iCmdShow)
{
    // create window
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, iCmdShow))
    {
        MessageBox(NULL, "This program error", szAppName, MB_ICONERROR);
        return 0;
    }

    MSG msg;
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
                
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // timer program;
        }
    }

    return msg.wParam;
}

void Show(HWND hwnd, HDC hdc, int xText, int yText, int iMapMode, char *szMapMode)
{
    char szBuffer[10];
    RECT rect;

    SaveDC(hdc);
    SetMapMode(hdc, iMapMode);
    GetClientRect(hwnd, &rect);
    DPtoLP(hdc, (PPOINT)&rect, 2);

    RestoreDC(hdc, -1);
    TextOut(hdc, xText, yText, szBuffer, wsprintf(szBuffer, "%-20s %7d %7d %7d %7d", szMapMode, rect.left, rect.right, rect.top, rect.bottom));
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int cxChar, cxCaps, cyChar;
    char szBuffer[10];
    RECT rect;

    HDC hdc;
    PAINTSTRUCT ps;
    TEXTMETRIC tm;

    static int cxClient, cyClient;

    static POINT apt[4];
    static POINT aptFigure[] = {10, 70, 50, 70, 50, 10, 90, 10, 90, 50, 30, 50, 30, 90,
                                70, 90, 70, 30, 10, 30};

    static TCHAR szHeading[] =
        TEXT("Mapping Mode   Left   Right  Top  Bottom");

    static TCHAR szUndLine[] =
        TEXT("------------   ----   -----   ---  ------");

    switch (message)
    {
    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);

        apt[0].x = cxClient / 4;
        apt[0].y = cyClient / 2;

        apt[1].x = cxClient / 2;
        apt[1].y = cyClient / 4;

        apt[2].x = cxClient / 2;
        apt[2].y = 3 * cyClient / 4;

        apt[3].x = 3 * cxClient / 4;
        apt[3].y = cyClient / 2;
        break;
    case WM_CREATE:
        hdc = GetDC(hwnd);

        GetTextMetrics(hdc, &tm);

        cxChar = tm.tmAveCharWidth;
        cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
        cyChar = tm.tmHeight + tm.tmExternalLeading;

        ReleaseDC(hwnd, hdc);
        break;

        // case WM_LBUTTONDOWN:
        // case WM_RBUTTONDOWN:
        // case WM_MOUSEMOVE:
        //     if(wParam & MK_LBUTTON || wParam & MK_RBUTTON)
        //     {
        //         hdc = GetDC(hwnd);

        //         SelectObject(hdc, GetStockObject(WHITE_PEN));
        //         DrawBezier(hdc, apt);

        //         if(wParam & MK_LBUTTON)
        //         {
        //             apt[1].x = LOWORD(lParam);
        //             apt[1].y = HIWORD(lParam);
        //         }
        //         if(wParam & MK_RBUTTON)
        //         {
        //             apt[2].x = LOWORD(lParam);
        //             apt[2].y = HIWORD(lParam);
        //         }
        //         SelectObject(hdc, GetStockObject(BLACK_PEN));
        //         DrawBezier(hdc, apt);

        //         ReleaseDC(hwnd, hdc);
        //     }
        //     break;

    case WM_PAINT:
    {
        InvalidateRect(hwnd, NULL, true);

        //start drawing on device context

        hdc = BeginPaint(hwnd, &ps);
        // for(int i = 0; i < NUMLINES; i++)
        // {
        //     TextOut(hdc, 0, cyChar * i, devcaps[i].szLabel, lstrlen(devcaps[i].szLabel));
        //     TextOut(hdc, 14 * cxCaps, cyChar * i, devcaps[i].szDesc, lstrlen(devcaps[i].szDesc));
        //     SetTextAlign(hdc, TA_RIGHT | TA_TOP);
        //     TextOut(hdc, 14 * cxCaps + 35 * cxChar, cyChar * i, szBuffer,
        //         wsprintf(szBuffer, "%5d", GetDeviceCaps(hdc, devcaps[i].iIndex)));
        //     SetTextAlign(hdc, TA_LEFT | TA_TOP);

        // }
        // int iBitsPixel = GetDeviceCaps(hdc, BITSPIXEL);
        // TextOut(hdc, 0, cyChar, szBuffer,
        //     wsprintf(szBuffer, "%5d", 1 << (GetDeviceCaps(hdc, PLANES) * GetDeviceCaps(hdc, BITSPIXEL))));

        // int x = rand() % (ps.rcPaint.bottom - ps.rcPaint.top);
        // for(int i = 0; i < 200; i++)
        // {
        //     SetPixel(hdc, x, ps.rcPaint.left + 200 + i, RGB(255, 255, 0));
        // }

        // draw a table
        // GetClientRect(hwnd, &rect);
        // for(int x = 0; x < rect.right; x += (rect.right - rect.left + 1) / 10)
        // {
        //     MoveToEx(hdc, x, 0, NULL);
        //     LineTo(hdc, x, rect.bottom);
        // }

        // for(int y = 0; y < rect.bottom; y += (rect.bottom - rect.top + 1) / 10)
        // {
        //     MoveToEx(hdc, 0, y, NULL);
        //     LineTo(hdc, rect.right, y);
        // }

        // draw a rectangle broad
        //POINT apt[5] = {100, 100, 200, 100, 200, 200, 100, 200, 100, 100};
        // MoveToEx(hdc, apt[0].x, apt[0].y, NULL);
        // for(int i = 1; i < 5; i++)
        // {
        //     LineTo(hdc, apt[i].x, apt[i].y);
        // }
        /*same as poly line*/
        // Polyline(hdc, apt, 5);

        // POINT apt2[5] = {300, 300, 400, 300, 400, 400, 300, 400, 300, 300};
        // // automatic link last point and first point(line last point will be used)
        // MoveToEx(hdc, apt2[0].x, apt2[0].y, NULL);
        // PolylineTo(hdc, apt2 + 1, 4);

#if 0
        // draw a sin wave
        int num = 1000;
        const double pi = 3.14159;
        POINT apt[1000];
        MoveToEx(hdc, 0, cyClient / 2, NULL);
        LineTo(hdc, cxClient, cyClient / 2);

        for(int i = 0; i < num; i++)
        {
            apt[i].x = cxClient / num * i;
            apt[i].y = (int)(cyClient / 2 + cyClient / 2 * sin(2 * pi * i / num));
        }
        Polyline(hdc, apt, num);
#endif

#if 0
        Rectangle(hdc, cxClient / 8, cyClient / 8, 7 * cxClient / 8, 7 * cyClient / 8);

        MoveToEx(hdc, 0, 0, NULL); LineTo(hdc, cxClient, cyClient);
        MoveToEx(hdc, cxClient, 0, NULL); LineTo(hdc, 0, cyClient);

        Ellipse(hdc, cxClient / 8, cyClient / 8, 7 * cxClient / 8, 7 * cyClient / 8);

        RoundRect(hdc, cxClient / 4, cyClient / 4, 3 * cxClient / 4, 3 * cyClient / 4, 
            cxClient / 4, cyClient / 4);
#endif

#if 0
        // Bezier curve
        DrawBezier(hdc, apt);
#endif

#if 0
        static HPEN hPen1, hPen2, hPen3;
        hPen1 = CreatePen(PS_DASHDOT, 1, RGB(0, 255, 255));
        SetBkColor(hdc, RGB(0, 255, 0));
        SelectObject(hdc, hPen1);
        // draw a table 
        GetClientRect(hwnd, &rect);
        for(int x = 0; x < rect.right; x += (rect.right - rect.left + 1) / 10)
        {
            MoveToEx(hdc, x, 0, NULL);
            LineTo(hdc, x, rect.bottom);
        }
        for(int y = 0; y < rect.bottom; y += (rect.bottom - rect.top + 1) / 10)
        {
            MoveToEx(hdc, 0, y, NULL);
            LineTo(hdc, rect.right, y);
        }

        DeleteObject(hPen1);

#endif
#if 0
        // fill or non fill
        // alternate and winding
        SelectObject(hdc, GetStockObject(GRAY_BRUSH));
        for(int i = 0; i < 10; i++)
        {
            /* according to scale zoom in and zome out*/
            apt[i].x = cxClient * aptFigure[i].x / 200;
            apt[i].y = cyClient * aptFigure[i].y / 100;
        }
        SetPolyFillMode(hdc, ALTERNATE);
        Polygon(hdc, apt, 10);

        for(int i = 0; i < 10; i++)
        {
            apt[i].x += cxClient / 2;
        }
        SetPolyFillMode(hdc, WINDING);

        
        Polygon(hdc, apt, 10);
#endif

#if 0
        SetBkMode(hdc, OPAQUE);
        HBRUSH hBursh = CreateHatchBrush(HS_FDIAGONAL, RGB(255, 255, 0));
        SelectObject(hdc, hBursh);
        Rectangle(hdc, 50, 20, 200, 100);
        DeleteObject(hBursh);

#endif
#if 0 // not process
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        SetMapMode(hdc, MM_ANISOTROPIC);
        SetWindowExtEx(hdc, 1, 1, NULL);
        SetViewportExtEx(hdc, cxChar, cyChar, NULL);

        TextOut(hdc, 1, 1, szHeading, lstrlen(szHeading));
        TextOut(hdc, 1, 2, szUndLine, lstrlen(szUndLine));

        Show(hwnd, hdc, 1, 3, MM_TEXT, TEXT("TEXT (pixels)"));

        Show(hwnd, hdc, 1, 4, MM_LOMETRIC, TEXT("LOMETRIC (.1 mm)"));

        Show(hwnd, hdc, 1, 5, MM_HIMETRIC, TEXT("HIMETRIC (.01 mm)"));

        Show(hwnd, hdc, 1, 6, MM_LOENGLISH, TEXT("LOENGLISH (.01 in)"));

        Show(hwnd, hdc, 1, 7, MM_HIENGLISH, TEXT("HIENGLISH (.001 in)"));

        Show(hwnd, hdc, 1, 8, MM_TWIPS, TEXT("TWIPS (1/1440 in)"));

#endif

        //stop drawing
        EndPaint(hwnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

void DrawBezier(HDC hdc, POINT apt[])
{
    PolyBezier(hdc, apt, 4);
    MoveToEx(hdc, apt[0].x, apt[0].y, NULL);
    LineTo(hdc, apt[1].x, apt[1].y);
    MoveToEx(hdc, apt[2].x, apt[2].y, NULL);
    LineTo(hdc, apt[3].x, apt[3].y);
}
