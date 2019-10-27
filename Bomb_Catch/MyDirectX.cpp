#include <iostream>
#include "MyDirectX.h"
using namespace std;

// Direct3D object
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;

/**
 ** Direct3D initialization
 **/
 //Direct3D function
bool Direct3D_Init(HWND window, int width, int height, bool fullscreen)
{
	//initialize Direct3D
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)
		return false;

	//set Direct3D presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = (!fullscreen);
	d3dpp.SwapEffect = D3DSWAPEFFECT_COPY;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;
	d3dpp.hDeviceWindow = window;

	//create Direct3D device
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (!d3ddev)
		return false;

	// get a pointer to the back buffer surface
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	return true;
}
void Direct3D_Shutdown()
{
	if (d3ddev)
		d3ddev->Release();
	if (d3d)
		d3d->Release();
}
/*
 loads a bitmap file into a surface
*/
LPDIRECT3DSURFACE9 LoadSurface(string filename)
{
	LPDIRECT3DSURFACE9 image = NULL;

	// get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return NULL;

	// create surface

	result = d3ddev->CreateOffscreenPlainSurface(
		info.Width, // width of the sruface
		info.Height, // height of the surface
		D3DFMT_X8R8G8B8, // surface format
		D3DPOOL_DEFAULT, // memory pool to use
		&image, // pointer to the surface
		NULL // reserved(always NULL)
	);
	if (result != D3D_OK)
		return NULL;
	//load surface from file into newly created surface
	result = D3DXLoadSurfaceFromFile(
		image, // destination surface
		NULL, // destination palette
		NULL, //desination rectangle
		filename.c_str(), // source filename
		NULL, // source rectangle
		D3DX_DEFAULT, // controls how image is filtered
		D3DCOLOR_XRGB(0, 0, 0), // for transparency(0 for none)
		NULL	// source image info(usually NULL)
	);
	if (result != D3D_OK)
		return NULL;

	return image;
}

/**
draw a surfac to the scren using StretchRect
**/
void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
	//get width & height from source surface
	D3DSURFACE_DESC desc;
	source->GetDesc(&desc);

	//create rects for drawing
	RECT source_rect = { 0, 0, (long)desc.Width, (long)desc.Height };
	RECT dest_rect = { (long)x, (long)y, (long)x + desc.Width, (long)y + desc.Height };

	//draw the source surface onto the dest
	d3ddev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);
}

//DirectInput objects, device and states
LPDIRECTINPUT8 dinput = NULL;
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
DIMOUSESTATE mouse_state;
char keys[256];
XINPUT_GAMEPAD controllers[4]; // used to save xinput_state

//DirectInput functions
bool DirectInput_Init(HWND hWnd)
{
	HRESULT result = DirectInput8Create(
		GetModuleHandle(NULL), //window(hwnd) instance handle
		DIRECTINPUT_VERSION, // direct input version
		IID_IDirectInput8, // reference label
		(void**)&dinput,
		NULL	// alaways NULL
	);
	// initialize the keyboard
	dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
	dikeyboard->SetDataFormat(&c_dfDIKeyboard);
	dikeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dikeyboard->Acquire();

	// initialize the mouse
	dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
	dimouse->SetDataFormat(&c_dfDIMouse);
	dimouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dimouse->Acquire();

	d3ddev->ShowCursor(false);

	return true;
}
void DirectInput_Update()
{
	//update mouse
	dimouse->GetDeviceState(sizeof(mouse_state), (LPVOID)&mouse_state);
	//update keyboard
	dikeyboard->GetDeviceState(sizeof(keys), (LPVOID)&keys);

	// update controllers
	for (int i = 0; i < 4; i++)
	{
		ZeroMemory(&controllers[i], sizeof(XINPUT_STATE));

		//get the state of the controllers
		XINPUT_STATE state;
		DWORD result = XInputGetState(i, &state);

		//store state in global control controllers array
		if (result == 0) controllers[i] = state.Gamepad;
	}
}
void DirectInput_Shutdown()
{
	if (dikeyboard)
	{
		dikeyboard->Unacquire();
		dikeyboard->Release();
		dikeyboard = NULL;
	}
	if (dimouse)
	{
		dimouse->Unacquire();
		dimouse->Release();
		dimouse = NULL;
	}
}
/* return key press state */
int Key_Down(int key)
{
	return (keys[key] & 0x80);
}
/* return mouse button state*/
int Mouse_Button(int button)
{
	return mouse_state.rgbButtons[button] & 0x80;
}
/* return mouse x movement */
int Mouse_X()
{
	return mouse_state.lX;
}
/* return mouse y movement */
int Mouse_Y()
{
	return mouse_state.lY;
}
void XInput_Vibrate(int contNum, int amount)
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = amount;
	vibration.wRightMotorSpeed = amount;
	XInputSetState(contNum, &vibration);
}
bool XInput_Controller_Found()
{
	XINPUT_CAPABILITIES caps;
	ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));
	XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps);
	if (caps.Type != 0) return false;

	return true;
}


