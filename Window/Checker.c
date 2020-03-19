#include <windows.h>
#define DIVISION 5

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
    static int cxClient, cyClient;
    static int cxBlock, cyBlock;
    PAINTSTRUCT ps;
    static BOOL fState[DIVISION][DIVISION];
    RECT rect;
    HDC hdc;
    POINT pt;
    int x = 0, y = 0;
    switch (message)
    {
    case WM_CREATE:
    case WM_DISPLAYCHANGE:

        hdc = GetDC(hwnd);
        SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
        ReleaseDC(hwnd, hdc);
        break;
    case WM_SIZE:
        if (message == WM_SIZE)
        {
            cyClient = HIWORD(lParam);
            cxClient = LOWORD(lParam);
        }
        cxBlock = cxClient / DIVISION;
        cyBlock = cyClient / DIVISION;

        return 0;
    case WM_SETFOCUS:
        ShowCursor(TRUE);
        return 0;
    case WM_KILLFOCUS:
        ShowCursor(FALSE);
        return 0;
    case WM_KEYDOWN:
        GetCursorPos(&pt);
        ScreenToClient(hwnd, &pt);
        x = max(0, min(DIVISION - 1, pt.x / cxBlock));
        y = max(0, min(DIVISION - 1, pt.y / cyBlock));
        switch(wParam)
        {
            case VK_UP: y--; break;
            case VK_DOWN: y++; break;
            case VK_LEFT: x--; break;
            case VK_RIGHT: x++; break;
            case VK_HOME: x = y = 0; break;
            case VK_END: x = y = DIVISION - 1; break;
            case VK_RETURN:
            case VK_SPACE: SendMessage(hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x * cxBlock, y * cyBlock));
                break;
        }
        x = (x + DIVISION) % DIVISION;
        y = (y + DIVISION) % DIVISION; // skill
        pt.x = x * cxBlock + cxBlock / 2;
        pt.y = y * cyBlock + cyBlock / 2;

        ClientToScreen(hwnd, &pt);
        SetCursorPos(pt.x, pt.y);
        return 0;
    case WM_LBUTTONDOWN:
        x = LOWORD(lParam) / cxBlock;
        y = HIWORD(lParam) / cyBlock;
        if(x < DIVISION && y < DIVISION)
        {
            fState[x][y] ^= 1;
            rect.left = x * cxBlock;
            rect.right = (x + 1) * cxBlock;
            rect.top = y * cyBlock;
            rect.bottom = (y + 1) * cyBlock;
            InvalidateRect(hwnd, &rect, FALSE);
        }
        else
        {
            MessageBeep(0);
        }
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        for(int x = 0; x < cxBlock; x++)
        {
            for(int y = 0; y < cyBlock; y++)
            {
                Rectangle(hdc, x * cxBlock, y * cyBlock, (x + 1) * cxBlock, (y + 1) * cyBlock);

                if(fState[x][y])
                {
                    MoveToEx(hdc, x * cxBlock, y * cyBlock, NULL);
                    LineTo(hdc, (x + 1) * cxBlock, (y + 1) * cyBlock);
                    MoveToEx(hdc, (x + 1) * cxBlock, y * cyBlock, NULL);
                    LineTo(hdc, x * cxBlock, (y + 1) * cyBlock);
                }
            }
            
        }
        EndPaint(hwnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

