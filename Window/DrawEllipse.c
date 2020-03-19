#include <windows.h>
#include <stdlib.h> // for the rand function
#include <math.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawRectangle(HWND);
int cxClient, cyClient;

#define TWO_PI 2 * 3.14159

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT("RandRect");
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

    hwnd = CreateWindow(szAppName, TEXT("Random Rectangles"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);


    // while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    // illage*, it will delete other message except WM_PAINT, program will drop in WM_PAINT
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }   // no message so print a rectangle
        else
        {

        }
    }
    return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    static HRGN hRgnClip;

    HDC hdc;
    HRGN hRgnTemp[6];
    PAINTSTRUCT ps;
    HCURSOR hCursor;
    
    switch (iMsg)
    {

    case WM_SIZE:
        cxClient = LOWORD(lParam);
        cyClient = HIWORD(lParam);

        hCursor = SetCursor(LoadCursor(NULL, IDC_WAIT));
        ShowCursor(TRUE);

        if(hRgnClip)
            DeleteObject(hRgnClip);

        hRgnTemp[0] = CreateEllipticRgn(0, cyClient / 3, cxClient / 2, 2 * cyClient / 3);
        hRgnTemp[1] = CreateEllipticRgn(cxClient / 2, cyClient / 3, cxClient, 2 * cyClient / 3);
        hRgnTemp[2] = CreateEllipticRgn(cxClient / 3, 0, 2 * cxClient / 3, cyClient / 2);
        hRgnTemp[3] = CreateEllipticRgn(cxClient / 3, cyClient / 2, 2 * cxClient / 3, cyClient);

        hRgnTemp[4] =  CreateRectRgn(0, 0, 1, 1);
        hRgnTemp[5] = CreateRectRgn(0, 0, 1, 1);
        hRgnClip = CreateRectRgn(0, 0, 1, 1);

        CombineRgn(hRgnTemp[4], hRgnTemp[0], hRgnTemp[1], RGN_OR);
        CombineRgn(hRgnTemp[5], hRgnTemp[2], hRgnTemp[3], RGN_OR);
        CombineRgn(hRgnClip, hRgnTemp[4], hRgnTemp[5], RGN_XOR);

        for(int i = 0; i < 6; i++)
            DeleteObject(hRgnTemp[i]);
        

        SetCursor(hCursor);
        ShowCursor(FALSE);
        
        break;

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);

        SetViewportOrgEx(hdc, cxClient / 2, cyClient / 2, NULL); // move origin to center
        SelectObject(hdc, hRgnClip); // select clipping region

        double fRadius = hypot(cxClient / 2, cyClient / 2);
        for(double fAngle = 0.0; fAngle < (360 * TWO_PI / 360); fAngle += TWO_PI / 360)
        {
            MoveToEx(hdc, 0, 0, NULL);
            LineTo(hdc, (int)(fRadius * cos(fAngle) + 0.5), (int)(fRadius * sin(fAngle) + 0.5));
        }
        

        EndPaint(hwnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

