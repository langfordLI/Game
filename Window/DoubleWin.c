#include <windows.h>
#define DIMISIONS 5

int idFocus = 0;
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
TCHAR szChildName[] = TEXT("DoubleWin");

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

    wndclass.lpfnWndProc = ChildWndProc;
    wndclass.cbWndExtra = sizeof(long);
    wndclass.hIcon = NULL;
    wndclass.lpszClassName = szChildName;
    RegisterClass(&wndclass);

    hwnd = CreateWindow(szAppName, TEXT("Checker3 Mouse Hit Test Demo"),
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
    PAINTSTRUCT ps;
    HDC hdc;
    static int cxBlock, cyBlock;
    static HWND hwndChild[DIMISIONS][DIMISIONS];
    static int x_set, y_set;
    switch (message)
    {
    case WM_CREATE:
        for (int x = cxBlock; x < DIMISIONS; x++)
        {
            for (int y = cyBlock; y < DIMISIONS; y++)
            {
                hwndChild[x][y] = CreateWindow(szChildName, NULL,
                                               WS_CHILDWINDOW | WS_VISIBLE,
                                               0, 0, 0, 0,
                                               hwnd, (HMENU)(y << 8 | x),
                                               (HINSTANCE)GetWindowLong(hwnd, GWL_HINSTANCE), NULL);
            }
        }
        break;
    case WM_SIZE:
        if (message == WM_SIZE)
        {
            cyClient = HIWORD(lParam);
            cxClient = LOWORD(lParam);
        }
        cxBlock = cxClient / DIMISIONS;
        cyBlock = cyClient / DIMISIONS;
        for (int x = 0; x < DIMISIONS; x++)
        {
            for (int y = 0; y < DIMISIONS; y++)
            {
                MoveWindow(hwndChild[x][y], x * cxBlock, y * cyBlock, cxBlock, cyBlock, TRUE);
            }
        }

        return 0;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hwnd, idFocus));
        return 0;
    case WM_KEYDOWN:
        // (HMENU)(y << 8 | x)
        x_set = idFocus & 0xFF;
        y_set = idFocus >> 8;
        switch (wParam)
        {
        case VK_UP:
            y_set--;
            break;
        case VK_DOWN:
            y_set++;
            break;
        case VK_LEFT:
            x_set--;
            break;
        case VK_RIGHT:
            x_set++;
            break;
        case VK_HOME:
            x_set = 0;
            y_set = 0;
            break;
        case VK_END:
            x_set = DIMISIONS - 1;
            y_set = DIMISIONS - 1;
            break;
        default:
            return 0;
        }
        x_set = (x_set + DIMISIONS) % DIMISIONS;
        y_set = (y_set + DIMISIONS) % DIMISIONS;
        idFocus = y_set << 8 | x_set;
        SetFocus(GetDlgItem(hwnd, idFocus));
        return 0;

    case WM_LBUTTONDOWN:
        MessageBeep(0);
        return 0;

    // case WM_PAINT:
    //     hdc = BeginPaint(hwnd, &ps);

    //     EndPaint(hwnd, &ps);
    //     return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ChildWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int cxClient, cyClient;
    PAINTSTRUCT ps;
    HDC hdc;
    RECT rect;
    switch (message)
    {
    case WM_CREATE:
        SetWindowLong(hwnd, 0, 0); // on/off flag, use register class wndclass.cbWndExtra = sizeof(long) save
        return 0;

    case WM_KEYDOWN:
        // send most key presses to the parent window
        if (wParam != VK_RETURN && wParam != VK_SPACE)
        {
            SendMessage(GetParent(hwnd), message, wParam, lParam);
            return 0;
        }
        // fall return and space, fall through to toggle the square

    case WM_LBUTTONDOWN:
        SetWindowLong(hwnd, 0, 1 ^ GetWindowLong(hwnd, 0));
        SetFocus(hwnd);
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;

    case WM_SETFOCUS:
        idFocus = GetWindowLong(hwnd, GWL_ID);
        // fall through
    case WM_KILLFOCUS:
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        GetClientRect(hwnd, &rect);
        Rectangle(hdc, 0, 0, rect.right, rect.bottom);
        if (GetWindowLong(hwnd, 0))
        {
            MoveToEx(hdc, 0, 0, NULL);
            LineTo(hdc, rect.right, rect.bottom);
            MoveToEx(hdc, rect.right, 0, NULL);
            LineTo(hdc, 0, rect.bottom);
        }
        if (hwnd == GetFocus())
        {
            rect.left += rect.right / 10;
            rect.right -= rect.left;
            rect.top += rect.bottom / 10;
            rect.bottom -= rect.top;
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            SelectObject(hdc, CreatePen(PS_DASH, 0, 0));
            Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
            DeleteObject(SelectObject(hdc, GetStockObject(BLACK_PEN)));
        }
        EndPaint(hwnd, &ps);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
