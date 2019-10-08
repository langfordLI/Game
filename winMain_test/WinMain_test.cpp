#include <Windows.h>
#include <iostream>
using namespace std;

const string ProgramTitle = "Hello Windows";

//windows event callback function
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	string text = "Hello Windows!";

	switch (message)
	{
	case WM_PAINT:
	{
		//get the dimensions of the window
		RECT rt;
		GetClientRect(hWnd, &rt);

		//start drawing on device context
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		//draw some text
		DrawText(hdc, text.c_str(), text.length(), &rt, DT_CENTER);

		//draw 1000 random pixels
		for (int n = 0; n < 3000; n++)
		{
			int x = rand() % (rt.right - rt.left);
			int y = rand() % (rt.bottom - rt.top);
			COLORREF c = RGB(rand() % 256, rand() % 256, rand() % 256);
			SetPixel(hdc, x, y, c);
		}

		//stop drawing 
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
	
}

//Helper function to set up the window properties

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	//set the new window's properties
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = ProgramTitle.c_str();
	wc.hIconSm = NULL;
	return RegisterClassEx(&wc);
}


//helper function to create the window and refresh it
bool InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//create a new window
	HWND hWnd = CreateWindow(
		ProgramTitle.c_str(), // window class
		ProgramTitle.c_str(), // title bar
		WS_OVERLAPPEDWINDOW,	// window style
		CW_USEDEFAULT, CW_USEDEFAULT, //postion of window
		640, 480,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	//was there an error creating the window?
	if (hWnd == 0)
		return 0;
	//display the window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return 1;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	////declare variable
	//MSG msg;

	//register the class
	MyRegisterClass(hInstance);

	//initialize application
	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	//main message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}