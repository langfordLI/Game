#include <windows.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("KeyView");
    HWND hwnd;
    MSG msg;
    WNDCLASS wndclass;

    wndclass.style = CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.hInstance = hInstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = szAppName;

    if (!RegisterClass(&wndclass))
    {
        MessageBox(NULL, TEXT("This program requires Windows NT!"),
                   szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindow(szAppName, TEXT("Keyboard Message Viewer #1"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int cxGrid, cyGrid;
    TEXTMETRIC tm;
    TCHAR szFaceName[LF_FACESIZE], szBuffer[LF_FACESIZE + 64];
    static struct
    {
        int idStockFont;
        TCHAR *szStockFont;
    } stockfont[] = {
        OEM_FIXED_FONT, "OEM_FIXED_FONT",
        ANSI_FIXED_FONT, "ANSI_FIXED_FONT",
        ANSI_VAR_FONT, "ANSI_VAR_FONT",
        SYSTEM_FONT, "SYSTEM_FONT",
        DEVICE_DEFAULT_FONT, "DEVICE_DEFAULT_FONT",
        SYSTEM_FIXED_FONT, "SYSTEM_FIXED_FONT",
        DEFAULT_GUI_FONT, "DEFAULT_GUI_FONT"
    };
    static int iFont, cFonts = sizeof(stockfont) / sizeof(stockfont[0]);
    static int cxClient, cyClient;
    PAINTSTRUCT ps;
    HDC hdc;
    switch (message)
    {
    case WM_CREATE:
        SetScrollRange(hwnd, SB_VERT, 0, cFonts - 1, TRUE);
        break;
    case WM_DISPLAYCHANGE:
        InvalidateRect(hwnd, NULL, TRUE);// InvalidateRect函数使矩形区域变得无效，从而引发重绘 
        break;
    case WM_SIZE:
        if (message == WM_SIZE)
        {
            cyClient = HIWORD(lParam);
            cxClient = LOWORD(lParam);
        }

        return 0;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        SelectObject(hdc, GetStockObject(stockfont[iFont].idStockFont));
        GetTextFace(hdc, LF_FACESIZE, szFaceName);
        GetTextMetrics(hdc, &tm);
        cxGrid = cxClient / 18;
        cyGrid = cyClient / 18;

        TextOut(hdc, 0, 0, szBuffer, 
            wsprintf(szBuffer, TEXT("%s: FAceName = %s, CharSet = %d"), 
                stockfont[iFont].szStockFont, szFaceName, tm.tmCharSet));

        SetTextAlign(hdc, TA_TOP | TA_CENTER);

        for(int i = 0; i < 18; i++)
        {
            MoveToEx(hdc, i * cxGrid, cyGrid, NULL);
            LineTo(hdc, i * cxGrid, 18 * cyGrid);

            MoveToEx(hdc, 0, (i + 1) * cyGrid, NULL);
            LineTo(hdc, 17 * cxGrid, (i + 1) * cyGrid);
        }
        SetBkMode(hdc, TRANSPARENT);
        for(int i = 0; i < 16; i++)
        {
            TextOut(hdc, (i + 1) * cxGrid + 20, cyGrid + 5, szBuffer,
                wsprintf(szBuffer, TEXT("%X-"), i));

            TextOut(hdc, 20, cyGrid * 2 + i * cyGrid + 10, szBuffer, 
                wsprintf(szBuffer, TEXT("-%X"), i));
        }

        for(int i = 0; i < 16; i++)
        {
            for(int j = 0; j < 16; j++)
            {
                TextOut(hdc, (j + 1) * cxGrid + 20, cyGrid * 2 + i * cyGrid + 10, szBuffer, 
                    wsprintf(szBuffer, TEXT("%c"), 16 * i + j));
            }
        }
        
        EndPaint(hwnd, &ps);

        return 0;
    case WM_VSCROLL:
        switch(wParam)
        {
            case SB_TOP: iFont = 0; break;
            case SB_BOTTOM: iFont = cFonts - 1; break;
            case SB_LINEUP:
            case SB_PAGEUP: iFont -= 1; break;
            case SB_LINEDOWN: iFont += 1; break;
            case SB_PAGEDOWN: iFont += 1; break;
            case SB_THUMBPOSITION: iFont = HIWORD(wParam); break;
        }
        iFont = max(0, min(cFonts - 1, iFont));
        SetScrollPos(hwnd, SB_VERT, iFont, TRUE);
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    case WM_KEYDOWN:
        switch(wParam)
        {
            case VK_HOME: SendMessage(hwnd, WM_VSCROLL, SB_TOP, 0); break;
            case VK_END: SendMessage(hwnd, WM_VSCROLL, SB_BOTTOM, 0); break;
            case VK_PRIOR: 
            case VK_LEFT:
            case VK_UP: SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, 0); break;
            case VK_NEXT:
            case VK_RIGHT:
            case VK_DOWN: SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, 0); break;
        }
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
