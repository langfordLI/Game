#include <Windows.h>
#include <time.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <iostream>	
#include <dinput.h>
#include <XInput.h>


using namespace std;

string text = "Hello Windows";
bool gameover = false;
const string APPTITLE = "Hello Windows";
HWND window;
const int SCREENW = 1024;
const int SCREENH = 768;

//Direct3D objects
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;

LPDIRECT3DSURFACE9 backbuffer = NULL;
LPDIRECT3DSURFACE9 surface = NULL;

HDC hdc = NULL;

ATOM MyRegisterClass(HINSTANCE hWnd);
bool InitInstanc(HINSTANCE hInstance, int nCmdShow);
bool Game_Init();
void Game_Run();
void Game_End();
bool XInput_Init(int contNum = 0);
void XInput_Update();
//void DrawBitmap(const char* filename, int x, int y);
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	MyRegisterClass(hInstance); // set parameter of the window
	
	if (!InitInstanc(hInstance, nCmdShow))
		return false;

	if (!Game_Init())
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

		Game_Run();
	}
	Game_End();
	
	return msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);	//
	wc.style = CS_HREDRAW | CS_VREDRAW;	 // when redraw the height and weight will change to redraw
	wc.lpfnWndProc = (WNDPROC)WinProc;;	//call back function(return LRESULT)
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0; // no other use, set 0 in general
	wc.hInstance = hInstance;	// main window point to a program window
	wc.hIcon = NULL; // import a icon as lago
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // import a cursor 
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); // window backgroud
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
bool Game_Init()
{
	//start up the random 
	//MessageBox(window, "Game_Init", "BREAKPOINT", 0);

	//initialize Direct3D
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3d == NULL)
	{
		MessageBox(window, "Error initializing Direct3D", "Error", MB_OK);
		return 0;
	}

	//replace HDC device use direct3d

	// set Direct3D presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = true; // window model set true, all screen model set false
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;	// back buffer set swap model
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;	// back buffer format
	d3dpp.BackBufferCount = 1; // back buffer count
	d3dpp.BackBufferWidth = SCREENW; // back buffer weight
	d3dpp.BackBufferHeight = SCREENH; // back buffer height
	d3dpp.hDeviceWindow = window; // the device father window(which to depend)

	//create Direct3D device
	d3d->CreateDevice(
		D3DADAPTER_DEFAULT, //use default video card
		D3DDEVTYPE_HAL, // use the hardware render
		window, // window handle, in original: device = GetDC(window);
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, /* do not use T&L(for compatibility)
									original: HDC hdcImage = CreateCompatibleDC(device)
											 SelectObject(hdcImage, image)*/
		&d3dpp,
		&d3ddev
	);
	

	if (d3ddev == NULL)
	{
		MessageBox(window, "Error creating Direct3D device", "Error", MB_OK);
		return false;
	}

	srand(time(NULL));
	
	//clear the backbuffer to black
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	// create pointer to the back buffer
	//d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	// create surface
	HRESULT result = d3ddev->CreateOffscreenPlainSurface(
		SCREENW, SCREENH, // width of the surface
		D3DFMT_X8R8G8B8, // sruface format, same as back buffer format
		D3DPOOL_DEFAULT, // use memory pool
		&surface, // pointer ot the surface
		NULL	// reserved(always NULL)
	);
	if (!SUCCEEDED(result))
		return false;

	RECT rect;
	rect.left = 100;
	rect.top = 90;
	rect.right = 500;
	rect.bottom = 500;

	//load surface from file into newly created surface
	result = D3DXLoadSurfaceFromFile(
		surface, // destination surface
		NULL, // destination palette
		NULL, // destination rectangle
		"legotron.bmp", // source filename
		NULL, // source rectangle
		D3DX_DEFAULT, // controls how image filtered
		0, // for transparency(0 for none)
		NULL // source image info (usually NULL)
	);

	/*
	********************************
	input device set
	********************************
	*/
	//// define a input device
	//LPDIRECTINPUT8 dinput;
	//HRESULT result = DirectInput8Create(
	//	GetModuleHandle(NULL), // current program instance handle
	//	DIRECTINPUT_VERSION, // direct input version
	//	IID_IDirectInput8, // reference idenifier
	//	(void**)&dinput, // main direct input device pointer
	//	NULL
	//);
	//LPDIRECTINPUTDEVICE8 diKeyBoard;
	//result = dinput->CreateDevice(
	//	GUID_SysKeyboard, // get direct input device handle
	//	&diKeyBoard, // device handle address pointer
	//	NULL
	//);
	//diKeyBoard->SetDataFormat(&c_dfDIKeyboard); // device type
	//diKeyBoard->SetCooperativeLevel(window, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND); // set cooperative level
	//diKeyBoard->Acquire(); // acquire device

	//char key[256];
	//diKeyBoard->GetDeviceState(sizeof(key), (LPVOID)&key);
	//// search key down
	//if (key[DIK_ESCAPE] & 0x80) {}


	/********************
	XInput device use method
	************************/
	XInput_Init();

	

	return true;
}

void Game_Run()
{
	//if (gameover == true) return;

	//RECT rect;
	//GetClientRect(window, &rect);

	//// draw bitmap 
	//int x = rand() % (rect.right - rect.left);
	//int y = rand() % (rect.bottom - rect.top);
	//DrawBitmap("c.bmp", x, y);

	// make sure the direct3d device is valid
	if (!d3ddev)
		return;

	////clear the backbuffer to bright green
	//d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 255, 0), 1.0f, 0);


	/* when copy a bitmap on surface, first create back buffer, then point to it*/
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);
	

	//start rendering
	if (d3ddev->BeginScene())
	{
		//int r = rand() % 255;
		//int g = rand() % 255;
		//int b = rand() % 255;
		//d3ddev->ColorFill(surface, NULL, D3DCOLOR_XRGB(r, g, b));

		//// copy the surface to the backbuffer
		//RECT rect;
		//rect.left = rand() % SCREENW / 2;
		//rect.right = rect.left + rand() % SCREENW / 2;
		//rect.top = rand() % SCREENH / 2;
		//rect.bottom = rect.right + rand() % SCREENH / 2;
		//d3ddev->StretchRect(surface, NULL, backbuffer, &rect, D3DTEXF_NONE); // drawing surface
		d3ddev->StretchRect(surface, NULL, backbuffer, NULL, D3DTEXF_NONE);

		//stop rendering
		d3ddev->EndScene();

		//copy back buffer on the screen
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	//check for escape key(to exit program)
	if (KEY_DOWN(VK_ESCAPE))
		PostMessage(window, WM_DESTROY, 0, 0);

	XInput_Update();
}

//void DrawBitmap(const char* filename, int x, int y)
//{
//	//load bitmap
//	HBITMAP	image = (HBITMAP)LoadImage(0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
//
//	//read bitmap's properties
//	BITMAP bm;
//	GetObject(image, sizeof(BITMAP), &bm);
//
//	//create a device context for the bitmap
//	HDC hdcImage = CreateCompatibleDC(device);
//	SelectObject(hdcImage, image);
//
//	// draw the bitmap to the window 
//	BitBlt(
//		device, //desination device context
//		x, y, // location on destination
//		bm.bmWidth, bm.bmHeight, // width, height of source bitmap
//		hdcImage, // source bitmap device context
//		0, 0, // start x, y on source bitmap
//		SRCCOPY // blit method
//	);
//
//	//delete the deevice context and bitmap
//	DeleteDC(hdcImage);
//	DeleteObject((HBITMAP)image);
//}
void Game_End()
{
	//// free the device
	//ReleaseDC(window, device);

	//display colse message
	MessageBox(window, "Program is about to end", "Game_End", MB_OK);

	//add surface release
	if (surface) surface->Release();
	//free memory
	if (d3ddev) d3ddev->Release();
	if (d3d) d3d->Release();
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

bool XInput_Init(int contNum)
{
	//	detect get xinput right or not
	XINPUT_CAPABILITIES caps;
	ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));
	XInputGetCapabilities(contNum, XINPUT_FLAG_GAMEPAD, &caps);
	if (caps.Type != XINPUT_DEVTYPE_GAMEPAD) return false;
	return true;
}
void XInput_Vibrate(int contNum = 0, int left = 65535, int right = 65535)
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(PXINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = left;
	vibration.wRightMotorSpeed = right;
	XInputSetState(contNum, &vibration);
}
void XInput_Update()
{
	RECT rect;
	GetClientRect(window, &rect);
	//start drawing on device context
	PAINTSTRUCT ps;
	hdc = BeginPaint(window, &ps);
	
	
	for (int i = 0; i < 4; i++)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		//get the state of the controller
		DWORD result = XInputGetState(i, &state);

		if (result == 0)
		{
			//controller is connected
			if (state.Gamepad.bLeftTrigger)
			{
				text = "Left Trigger";
			}
			else if (state.Gamepad.bRightTrigger)
			{
				text = "Right Trigger";
			}
			else if (state.Gamepad.sThumbLX < -10000 || state.Gamepad.sThumbLX > 10000)
			{
				text = "Left Thumb Stick";
				
			}
			else if (state.Gamepad.sThumbRX < -10000 || state.Gamepad.sThumbRX > 10000)
			{
				text = "Right Thumb Stick";
				
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
			{
				text = "DPAD UP";
				
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
			{
				text = "DPAD DOWN";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
			{
				text = "DPAD LEFT";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
			{
				text = "DPAD RIGHT";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START)
			{
				text = "DPAD RIGHT";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)
			{
				text = "LEFT THUMB";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB)
			{
				text = "RIGHT THUMB";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)
			{
				text = "LEFT SHOULDER";

			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER)
			{
				text = "RIGHT SHOULDER";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A)
			{
				text = "A";
				XInput_Vibrate(0, 65536, 65535);
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
			{
				text = "B";
				XInput_Vibrate(0, 0, 0);
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
			{
				text = "X";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
			{
				text = "Y";
			}
			else if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
			{
				gameover = true;
			}
		}
		else
		{
			//controler is not connected
		}
		DrawText(hdc, text.c_str(), text.length(), &rect, DT_CENTER);
	}
	EndPaint(window, &ps);
}