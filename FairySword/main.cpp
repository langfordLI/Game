#include <Windows.h>
#include <iostream>
#include <d3d10.h>
#include <dinput.h>
#include <time.h>
#include <tchar.h>
using namespace std;

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "MSImg32.lib")

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"core framework"

HDC g_hdc = NULL;
HDC g_mdc = NULL;
HDC g_bufdc = NULL;

struct Sprites
{
	int x, y; // the sit of the sprite
	// int direction; 
	bool exist;
};
Sprites sprite[30];
Sprites g_human;
HBITMAP g_hCharacter;
HBITMAP g_hSword;
HBITMAP g_hBackground;
HFONT g_hFont;
int g_iNum;
int g_iSwordNum = 0;
DWORD g_tNow = 0, g_tPre = 0;
int g_iBGOffest = 0;
int g_iX = 0, g_iY = 0;	// which express the current mouse position
int g_iXNow = 0, g_iYNow = 0; // which is slower than the mouse position




bool Game_Init(HWND hwnd);
void Game_Paint(HWND hwnd);
bool Game_CleanUp(HWND hwnd);


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void BubSort(int n);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX myClass = { 0 };
	myClass.cbSize = sizeof(WNDCLASSEX);
	myClass.style = CS_HREDRAW | CS_VREDRAW;
	myClass.lpfnWndProc = WndProc;
	myClass.cbClsExtra = 0; // The number of extra bytes to allocate following the window-class structure.
	myClass.cbWndExtra = 0; // The number of extra bytes to allocate following the window instance.
	myClass.hInstance = hInstance;
	//B myClass.hIcon = LoadIcon(NULL, L"icon.ico");
	myClass.hIcon = (HICON)LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0,
		LR_DEFAULTSIZE | LR_LOADFROMFILE);
	myClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	myClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	myClass.lpszMenuName = NULL;
	myClass.lpszClassName = L"For the dream of game development";

	if (!RegisterClassEx(&myClass))
		return -1;

	HWND hwnd = CreateWindow(L"For the dream of game development", // same as first regisiter lpszClassNmae
		WINDOW_TITLE, WS_OVERLAPPEDWINDOW, // window style
		CW_USEDEFAULT, CW_USEDEFAULT,	// window situation
		WINDOW_WIDTH, WINDOW_HEIGHT,	//
		NULL, NULL, hInstance, NULL
	);

	MoveWindow(hwnd, 250, 80, WINDOW_WIDTH, WINDOW_HEIGHT, true);
	ShowWindow(hwnd, nShowCmd); // nShowCmd how to show SW_...
	UpdateWindow(hwnd);

	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"initial source failed", L"MessageBox", 0);
		return false;
	}

	PlaySound(L"生生世世爱.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))	// PM_REMOVE will remove the message
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_tNow = GetTickCount();
			if (g_tNow - g_tPre >= 5)
			{
				Game_Paint(hwnd);
			}
		}
	}

	//while (GetMessage(&msg, 0, 0, 0))
	//{
	//	TranslateMessage(&msg);
	//	DispatchMessageW(&msg);
	//}

	

	UnregisterClass(L"For the dream of game development", myClass.hInstance);
	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_MOUSEMOVE:
		g_iX = LOWORD(lParam);
		if (g_iX > WINDOW_WIDTH - 317)
			g_iX = WINDOW_WIDTH - 317;
		else if (g_iX < 0)
			g_iX = 0;

		g_iY = HIWORD(lParam);
		if (g_iY > WINDOW_HEIGHT - 283)
			g_iY = WINDOW_HEIGHT - 283;
		else if (g_iY < 0)
			g_iY = 0;
		break;

	case WM_LBUTTONDOWN:
		for (int i = 0; i < 30; i++)
		{
			// cycle 30 sword to record. when there is a sword is not existed, input to array
			if (sprite[i].exist == false)
			{
				sprite[i].x = g_iXNow;
				sprite[i].y = g_iYNow;
				sprite[i].exist = true;
				g_iSwordNum++;
				break;
			}
		}
		break;

	case WM_DESTROY:
		Game_CleanUp(hwnd);
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			PostQuitMessage(0);
			break;
		}
		break;

	default:
		return DefWindowProcA(hwnd, message, wParam, lParam);
	}

	return 0;
}
void BubSort(int n)
{
	bool flag = true;
	for (int i = 0; i < n - 1; i++)
	{
		flag = false;
		for (int j = 0; j < n - i - 1; j++)
		{
			if (sprite[j].y > sprite[j + 1].y)
			{
				flag = true;
				Sprites temp = sprite[j + 1];
				sprite[j + 1] = sprite[j];
				sprite[j] = temp;
			}
		}
		if (flag == false)
			break;
	}
}
bool Game_Init(HWND hwnd)
{
	srand((unsigned)time(NULL));

	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);
	g_bufdc = CreateCompatibleDC(g_hdc);


	/*initial g_mdc*/
	HBITMAP bmp;
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);
	SelectObject(g_mdc, bmp);

	g_hBackground = (HBITMAP)LoadImage(NULL, L"bg.bmp", IMAGE_BITMAP, WINDOW_WIDTH, WINDOW_HEIGHT, LR_LOADFROMFILE);
	/* express up down left and right */
	g_hCharacter = (HBITMAP)LoadImage(NULL, L"swordman.bmp", IMAGE_BITMAP, 317, 283, LR_LOADFROMFILE);
	g_hSword = (HBITMAP)LoadImage(NULL, L"swordblade.bmp", IMAGE_BITMAP, 100, 26, LR_LOADFROMFILE);

	g_hFont = CreateFont(30, 0, 0, 0, 0, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, L"Times New Roman");

	/* restrict mouse postion */
	POINT pt, lt, rb;
	RECT rect;
	pt.x = 300;
	pt.y = 100;
	ClientToScreen(hwnd, &pt);
	SetCursorPos(pt.x, pt.y); // initial a sit of cursor
	
	ShowCursor(false);

	GetClientRect(hwnd, &rect);
	lt.x = rect.left;
	lt.y = rect.top;
	rb.x = rect.right;
	rb.y = rect.bottom;
	ClientToScreen(hwnd, &lt);
	ClientToScreen(hwnd, &rb);
	rect.left = lt.x;
	rect.top = lt.y;
	rect.right = rb.x;
	rect.bottom = rb.y;
	ClipCursor(&rect);

	g_iNum = 0;
	Game_Paint(hwnd);
	// ReleaseDC(hwnd, g_hdc);
	return true;
}




void Game_Paint(HWND hwnd)
{
	
	if (g_iNum == 8)
		g_iNum = 0; // if num super than 11, return to 0


	/* this transparent copy all background and human in h_mdc*/
	SelectObject(g_bufdc, g_hBackground);
	// draw background[0, window_weight] of last [window_weight - g_iBGOffset, window_weight] to
	// first [0, g_iBGOffset]
	BitBlt(g_mdc, 0, 0, g_iBGOffest, WINDOW_HEIGHT, g_bufdc, WINDOW_WIDTH - g_iBGOffest, 0, SRCCOPY);
	BitBlt(g_mdc, g_iBGOffest, 0, WINDOW_WIDTH - g_iBGOffest, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	// draw sword
	SelectObject(g_bufdc, g_hSword);
	if (g_iSwordNum != 0)
	{
		for (int i = 0; i < 30; i++)
		{
			if (sprite[i].exist == true)
			{
				TransparentBlt(g_mdc, sprite[i].x - 100, sprite[i].y + 100, 100, 26, 
					g_bufdc, 0, 0, 100, 26, RGB(0, 0, 0));

				sprite[i].x -= 10;
				if (sprite[i].x < 0)
				{
					sprite[i].exist = false;
					g_iSwordNum--;
				}
			}
		}
	}

	if (g_iXNow < g_iX)
	{
		g_iXNow += 10;
		if (g_iXNow > g_iX)
			g_iXNow = g_iX;
	}
	else
	{
		g_iXNow -= 10;
		if (g_iXNow < g_iX)
			g_iXNow = g_iX;
	}

	if (g_iYNow < g_iY)
	{
		g_iYNow += 10;
		if (g_iYNow > g_iY)
			g_iYNow = g_iY;
	}
	else
	{
		g_iYNow -= 10;
		if (g_iYNow < g_iY)
			g_iYNow = g_iY;
	}

	SelectObject(g_bufdc, g_hCharacter);
	TransparentBlt(g_mdc, g_iXNow, g_iYNow, 317, 283, g_bufdc, 0, 0, 317, 283, RGB(0, 0, 0));

	SelectObject(g_mdc, g_hFont);
	SetBkMode(g_mdc, TRANSPARENT);
	SetTextColor(g_mdc, RGB(255, 255, 0));
	wchar_t str[20] = {};
	swprintf_s(str, L"position x: %d", g_iX);
	TextOut(g_mdc, 0, 0, str, wcslen(str));
	swprintf_s(str, L"position y: %d", g_iY);
	TextOut(g_mdc, 0, 30, str, wcslen(str));
	
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount();

	g_iNum++;
	g_iBGOffest += 5;
	if (g_iBGOffest == WINDOW_WIDTH)
		g_iBGOffest = 0;
	
}
bool Game_CleanUp(HWND hwnd)
{
	/*for (int i = 0; i < 7; i++)
	{
		DeleteObject(g_hPen[i]);
		DeleteObject(g_hBrush[i]);
	}*/
	// KillTimer(hwnd, 1);
	
	DeleteObject(g_hBackground);
	DeleteObject(g_hCharacter);
	DeleteObject(g_hSword);

	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd, g_hdc);
	return true;
}



