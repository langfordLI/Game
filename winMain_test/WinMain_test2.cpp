#include <Windows.h>
#include <iostream>

#include <time.h>
using namespace std;

const string APPTITLE = "Game Loop";
HWND window;
HDC device;
bool gameover = false;

void DrawBitmap(const char* filename, int x, int y)
{
	//load the bitmap image;
	HBITMAP image = (HBITMAP)LoadImage(0, "c.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	//read the bitmap's properties
	BITMAP bm;
	GetObject(image, sizeof(BITMAP), &bm);

	//create a device context for the bitmap
	HDC hdcImage = CreateCompatibleDC(device);
	SelectObject(hdcImage, image);

	//draw the bitmap to the window(bit block transfer)
	BitBlt(
		device, // destination device context
		x, y, // x, y location on destination
		bm.bmWidth, bm.bmHeight, //width, height of source bitmap
		hdcImage, // source bitmap device context
		0, 0, //start x, y on source bitmap
		SRCCOPY
	);

	//delete the device context and bitmap
	DeleteDC(hdcImage);
	DeleteObject((HBITMAP)image);
}

//Startup and loading code goes here
bool Game_Init()
{
	//start up the random number generator
	srand(time(NULL));
	return 1;
}

//update function called from inside game loop
void Game_Run()
{
	if (gameover == true)
		return;
	//get the drawing surface
	RECT rect;
	GetClientRect(window, &rect);
	
	//draw bitmap at random location
	int x = rand() % (rect.right - rect.left);
	int y = rand() % (rect.bottom - rect.top);
	DrawBitmap("c.bmp", x, y);
}

//shutdown code
void Game_End()
{
	//free the device
	ReleaseDC(window, device);
}



//windows event callback function
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	string text = "Hello Windows!";

	switch (message)
	{
	

	case WM_DESTROY:
		gameover = true;
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
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = APPTITLE.c_str();
	wc.hIconSm = NULL;
	return RegisterClassEx(&wc);
}


//helper function to create the window and refresh it
bool InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//create a new window
	window = CreateWindow(
		APPTITLE.c_str(), // window class
		APPTITLE.c_str(), // title bar
		WS_OVERLAPPEDWINDOW,	// window style
		CW_USEDEFAULT, CW_USEDEFAULT, //postion of window
		640, 480,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	//was there an error creating the window?
	if (window == 0)
		return 0;
	//display the window
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	//get device context for drawing
	device = GetDC(window);

	return 1;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	////declare variable
	MSG msg;

	//register the class
	MyRegisterClass(hInstance);

	//initialize application
	if (!InitInstance(hInstance, nCmdShow))
		return FALSE;

	if (!Game_Init())
		return 0;

	//main message loop

	while (!gameover)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//process game loop
		Game_Run();
	}

	//free game resources
	Game_End();
	return msg.wParam;
}
