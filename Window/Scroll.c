#include <windows.h>
#include <stdio.h>
#include "systems.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
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
        wndclass.lpszClassName = "HelloWin";

        if (!RegisterClass(&wndclass))
        {
                MessageBox(NULL, "This program requires WindowsNT!",
                           "HelloWin", MB_ICONERROR);
                return 0;
        }
        /*check last window register success or not*/
        // if(!hPrevInstance)
        // {
        //         wndclass.cbStyle = CS_HREDRAW | CS_VERDRAW;
        //         ....
        //         RegisiterClass(&wndClass);
        // }
        HWND hwnd = CreateWindow(
            "HelloWin",                       // window class name
            "The Hello Program",              // window caption
            WS_OVERLAPPEDWINDOW | WS_VSCROLL, // window style over lapped window
            CW_USEDEFAULT,                    // initial x position use default
            CW_USEDEFAULT,                    // initial y position
            CW_USEDEFAULT,                    // initial x size
            CW_USEDEFAULT,                    // initial y size
            NULL,                             //parent window handle
            NULL,                             // window menu handle
            hInstance,                        // program instance handle
            NULL                              // create parameter
        );

        ShowWindow(hwnd, iCmdShow);
        UpdateWindow(hwnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0))
        {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
        }
        return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
        HDC hdc = NULL;
        PAINTSTRUCT ps;
        RECT rect;
        static int cxChar, cyChar, cxCaps, iMaxWidth; // average char width and total char height(c - count)
        CHAR szBuffer[10];
        TEXTMETRIC tm;
        static int cxClient, cyClient;
        static int iVscrollPos, iHscrollPos;
        SCROLLINFO si;
        int iBegin, iEnd;
        switch (message)
        {
        case WM_CREATE:
                // when create window get character width and height
                hdc = GetDC(hwnd);
                GetTextMetrics(hdc, &tm);
                cxChar = tm.tmAveCharWidth;
                cyChar = tm.tmHeight + tm.tmExternalLeading;
                // judge font is variable width or fixed width
                // variable width cxCaps = cxChar * 1.5
                // fixed width cxCaps = cxChar
                cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
                ReleaseDC(hwnd, hdc);

                iMaxWidth = 30 * cxCaps + 40 * cxChar;
                SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE);
                SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
                return 0;
#if 0
                case WM_PAINT:
                        // when window change, this command automatically restore paint
                        hdc = BeginPaint(hwnd, &ps);
                        
                        for(int i = 0;i < NUMLINES; i++)
                        {
                                /*
                                when draw window, extent part will fall into out windows,
                                such as cyChar * (i - 20), utill i add to 20 draw will show
                                */
                                
                                int y = cyChar * (i - iVscrollPos);
                                TextOut(hdc, 0, y, sysmetrics[i].szLabel, 
                                        strlen(sysmetrics[i].szLabel));
                                TextOut(hdc, 30 * cxChar, y, sysmetrics[i].szDesc, 
                                        strlen(sysmetrics[i].szDesc));
                                SetTextAlign(hdc, TA_RIGHT | TA_TOP);
                                TextOut(hdc, 30 * cxCaps + 40 * cxChar, y, szBuffer, 
                                        sprintf(szBuffer, "%5d", 
                                                GetSystemMetrics(sysmetrics[i].Index)));

                                // set for first sectence
                                SetTextAlign(hdc, TA_LEFT | TA_TOP);
                        }

                        EndPaint(hwnd, &ps);
                        return 0;
                


                case WM_SIZE:
                        cxClient = LOWORD(lParam);
                        cyClient = HIWORD(lParam);

                        return 0;
                case WM_VSCROLL:
                        switch(LOWORD(wParam))
                        {
                                case SB_LINEUP: iVscrollPos -= 1; break;                                    
                                case SB_LINEDOWN: iVscrollPos += 1; break;
                                case SB_PAGEUP: iVscrollPos -= cyClient / cyChar; break;
                                case SB_PAGEDOWN: iVscrollPos += cyClient / cyChar; break;
                                case SB_THUMBPOSITION: iVscrollPos = HIWORD(wParam); break;
                                default:
                                        break;
                        }
                        iVscrollPos = max(0, min(iVscrollPos, NUMLINES - 1));
                        if(iVscrollPos != GetScrollPos(hwnd, SB_VERT))
                        {
                                SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
                                InvalidateRect(hwnd, NULL, TRUE);
                        }
                        return 0;
#endif
#if 1
        case WM_SIZE:
                cxClient = LOWORD(lParam);
                cyClient = HIWORD(lParam);
                // set vertical scroll range and page size
                si.cbSize = sizeof(SCROLLINFO);
                si.fMask = SIF_RANGE | SIF_PAGE;
                si.nMin = 0;
                si.nMax = NUMLINES - 1;
                si.nPage = cyClient / cyChar;
                SetScrollInfo(hwnd, SB_VERT, &si, TRUE);

                // set horizontal scroll bar range and page size
                si.cbSize = sizeof(si);
                si.fMask = SIF_RANGE | SIF_PAGE;
                si.nMin = 0;
                si.nMax = 2 + iMaxWidth / cxChar;
                si.nPage = cxClient / cxChar;
                SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
                return 0;
        case WM_VSCROLL:
                // get all the vertical scroll bar informationg
                si.cbSize = sizeof(si);
                si.fMask = SIF_ALL;
                GetScrollInfo(hwnd, SB_VERT, &si);
                // save the current positon for comparison later on
                iVscrollPos = si.nPos;
                switch (LOWORD(wParam))
                {
                case SB_TOP:
                        si.nPos = si.nMin;
                        break;
                case SB_BOTTOM:
                        si.nPos = si.nMax;
                        break;
                case SB_LINEUP:
                        si.nPos -= 1;
                        break;
                case SB_LINEDOWN:
                        si.nPos += 1;
                        break;
                case SB_PAGEUP:
                        si.nPos -= si.nPage;
                        break;
                case SB_PAGEDOWN:
                        si.nPos += si.nPage;
                        break;
                case SB_THUMBTRACK:
                        si.nPos = si.nTrackPos;
                        break;
                default:
                        break;
                }
                // set the position and then retrieve it.
                // Due to adjustments by windows it may not be the same as
                // the value set
                si.fMask = SIF_POS;
                SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
                GetScrollInfo(hwnd, SB_VERT, &si);

                // if the positin has changed, scroll the window and update it
                if (si.nPos != iVscrollPos)
                {
                        ScrollWindow(hwnd, 0, cyChar * (iVscrollPos - si.nPos),
                                     NULL, NULL);
                        UpdateWindow(hwnd);
                }
                return 0;
        case WM_HSCROLL:
                // get all the vertical scroll bar informationg
                si.cbSize = sizeof(si);
                si.fMask = SIF_ALL;
                GetScrollInfo(hwnd, SB_HORZ, &si);
                // save the current positon for comparison later on
                iHscrollPos = si.nPos;
                switch (LOWORD(wParam))
                {
                case SB_LEFT:
                        si.nPos = si.nMin;
                        break;
                case SB_RIGHT:
                        si.nPos = si.nMax;
                        break;
                case SB_LINELEFT:
                        si.nPos -= 1;
                        break;
                case SB_LINERIGHT:
                        si.nPos += 1;
                        break;
                case SB_PAGERIGHT:
                        si.nPos -= si.nPage;
                        break;
                case SB_PAGELEFT:
                        si.nPos += si.nPage;
                        break;
                case SB_THUMBTRACK:
                        si.nPos = si.nTrackPos;
                        break;
                default:
                        break;
                }
                // set the position and then retrieve it.
                // Due to adjustments by windows it may not be the same as
                // the value set
                si.fMask = SIF_POS;
                SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
                GetScrollInfo(hwnd, SB_HORZ, &si);

                // if the positin has changed, scroll the window and update it
                if (si.nPos != iHscrollPos)
                {
                        ScrollWindow(hwnd, cxChar * (iHscrollPos - si.nPos), 0, 
                                     NULL, NULL);
                        UpdateWindow(hwnd);
                }
                return 0;
        case WM_PAINT:
                hdc = BeginPaint(hwnd, &ps);
                // get vertical scroll bar position
                si.cbSize = sizeof(si);
                si.fMask = SIF_ALL;
                GetScrollInfo(hwnd, SB_VERT, &si);
                iVscrollPos = si.nPos;

                // get horizontal scroll bar position
                si.cbSize = sizeof(si);
                si.fMask = SIF_ALL;
                GetScrollInfo(hwnd, SB_HORZ, &si);
                iHscrollPos = si.nPos;

                // find painting limit
                // descrip number
                iBegin = max(0, iVscrollPos + ps.rcPaint.top / cyChar);
                iEnd = min(NUMLINES - 1, iVscrollPos + ps.rcPaint.bottom / cyChar);

                for(int i = iBegin; i <= iEnd; i++)
                {
                        int x = cxChar * (1 - iHscrollPos);
                        int y = cyChar * (i - iVscrollPos);
                        TextOut(hdc, x, y, sysmetrics[i].szLabel, strlen(sysmetrics[i].szLabel));
                        TextOut(hdc, x + 30 * cxCaps, y, sysmetrics[i].szDesc, 
                                strlen(sysmetrics[i].szDesc));
                        SetTextAlign(hdc, TA_RIGHT | TA_TOP);
                        TextOut(hdc, x + 30 * cxCaps + 40 * cxChar, y, 
                                szBuffer, sprintf(szBuffer, 
                                        "%5d", GetSystemMetrics(sysmetrics[i].Index)));
                        SetTextAlign(hdc, TA_LEFT | TA_TOP);
                }

                EndPaint(hwnd, &ps);
                return 0;
#endif
        case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProc(hwnd, message, wParam, lParam);
}
