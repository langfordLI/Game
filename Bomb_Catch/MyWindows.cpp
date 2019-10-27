#include "MyDirectX.h"
#include <iostream>

using namespace std;
HWND window = NULL;
bool gameover = false;

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
ATOM MyRegisterClass(HINSTANCE hInstance);
bool InitInstanc(HINSTANCE hInstance, int nCmdShow);

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);	//
	wc.style = CS_HREDRAW | CS_VREDRAW;	 // when redraw the height and weight will change to redraw
	wc.lpfnWndProc = (WNDPROC)WinProc; //call back function(return LRESULT)
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0; // no other use, set 0 in general
	wc.hInstance = hInstance;	// main window point to a program window
	wc.hIcon = NULL; // import a icon as lago
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // import a cursor 
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); // window backgroud
	wc.lpszMenuName = NULL; // menu name of the program
	wc.lpszClassName = APPTITLE.c_str(); // point name for window
	wc.hIconSm = NULL;	// 

	return RegisterClassEx(&wc); // successful regisiter windows
}
bool InitInstanc(HINSTANCE hInstance, int nCmdShow)
{
	//create a new window
	window = CreateWindow(
		APPTITLE.c_str(), //window class
		APPTITLE.c_str(), // title bar
		WS_OVERLAPPEDWINDOW, // window style
		CW_USEDEFAULT, CW_USEDEFAULT, // position of window
		SCREENW, SCREENH, // dimensions of the window
		NULL, // parent window(not used)
		NULL, // menu(not used)
		hInstance, // application instance
		NULL	// window parameter(not used)
	);


	//was there an error creating the window, exit
	if (window == 0) return 0;

	// display the window
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	// use direct3d replace HDC device
	/*// get devicee context for drawing
	device = GetDC(window);*/

	return 1;
}

LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		gameover = true;
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	MyRegisterClass(hInstance); // set parameter of the window

	if (!InitInstanc(hInstance, nCmdShow))
		return false;

	if (!Game_Init(window))
	{
		return 0;
	}

	while (!gameover)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageA(&msg);
		}

		Game_Run(window);
	}
	Game_End();

	return msg.wParam;
}