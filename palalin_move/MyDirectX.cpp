/*  Beginning Game Programming, Third Edition
	MyDirectX.cpp

	Changes to make during AR:
		ch6 DirectInput_Update change back in ch5
		ch6 KeyDown change back in ch5
		ch6 return value of Key_Down

*/

#include "MyDirectX.h"
#include <iostream>
using namespace std;

//Direct3D variables
LPDIRECT3D9 d3d = NULL;
LPDIRECT3DDEVICE9 d3ddev = NULL;
LPDIRECT3DSURFACE9 backbuffer = NULL;

//*** ADDED
LPD3DXSPRITE spriteobj = nullptr;

//DirectInput variables
//edited
LPDIRECTINPUT8 dinput = NULL;
//IDirectInput8* dinput = NULL;

//edited
LPDIRECTINPUTDEVICE8 dimouse = NULL;
LPDIRECTINPUTDEVICE8 dikeyboard = NULL;
//IDirectInputDevice8* dimouse = NULL;
//IDirectInputDevice8* dikeyboard = NULL;


DIMOUSESTATE mouse_state;
char keys[256];
XINPUT_GAMEPAD controllers[4];


bool Direct3D_Init(HWND window, int width, int height, bool fullscreen)
{
	//initialize Direct3D
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d) return false;

	//set Direct3D presentation parameters
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.hDeviceWindow = window;
	d3dpp.Windowed = (!fullscreen);
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.EnableAutoDepthStencil = 1;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = width;
	d3dpp.BackBufferHeight = height;

	//create Direct3D device
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	if (!d3ddev) return false;


	//get a pointer to the back buffer surface
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	//*** ADDED
		//create sprite object
	HRESULT result = D3DXCreateSprite(d3ddev, &spriteobj);

	//if (result != D3D_OK)
	//	return 0;
	


	return 1;
}

void Direct3D_Shutdown()
{
	//*** ADDED
	if (spriteobj) spriteobj->Release();

	if (d3ddev) d3ddev->Release();
	if (d3d) d3d->Release();
}

void DrawSurface(LPDIRECT3DSURFACE9 dest, float x, float y, LPDIRECT3DSURFACE9 source)
{
	//get width/height from source surface
	D3DSURFACE_DESC desc;
	source->GetDesc(&desc);

	//create rects for drawing
	RECT source_rect = { 0, 0, (long)desc.Width, (long)desc.Height };
	RECT dest_rect = { (long)x, (long)y, (long)x + desc.Width, (long)y + desc.Height };

	//draw the source surface onto the dest
	d3ddev->StretchRect(source, &source_rect, dest, &dest_rect, D3DTEXF_NONE);

}

LPDIRECT3DSURFACE9 LoadSurface(string filename)
{
	LPDIRECT3DSURFACE9 image = NULL;

	//get width and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK)
		return NULL;

	//create surface
	result = d3ddev->CreateOffscreenPlainSurface(
		info.Width,         //width of the surface
		info.Height,        //height of the surface
		D3DFMT_X8R8G8B8,    //surface format
		D3DPOOL_DEFAULT,    //memory pool to use
		&image,             //pointer to the surface
		NULL);              //reserved (always NULL)

	if (result != D3D_OK) return NULL;

	//load surface from file into newly created surface
	result = D3DXLoadSurfaceFromFile(
		image,                  //destination surface
		NULL,                   //destination palette
		NULL,                   //destination rectangle
		filename.c_str(),       //source filename
		NULL,                   //source rectangle
		D3DX_DEFAULT,           //controls how image is filtered
		D3DCOLOR_XRGB(0, 0, 0),   //for transparency (0 for none)
		NULL);                  //source image info (usually NULL)

	//make sure file was loaded okay
	if (result != D3D_OK) return NULL;

	return image;
}

bool DirectInput_Init(HWND hwnd)
{
	//initialize DirectInput object
	DirectInput8Create(
		GetModuleHandle(NULL),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&dinput,
		NULL);

	//initialize the keyboard
	dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, NULL);
	dikeyboard->SetDataFormat(&c_dfDIKeyboard);
	dikeyboard->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dikeyboard->Acquire();

	//initialize the mouse
	dinput->CreateDevice(GUID_SysMouse, &dimouse, NULL);
	dimouse->SetDataFormat(&c_dfDIMouse);
	dimouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dimouse->Acquire();
	d3ddev->ShowCursor(false);

	return true;
}

void DirectInput_Update()
{
	//*** CHANGED in ch6

		//update mouse
	dimouse->Poll();
	if (!SUCCEEDED(dimouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_state)))
	{
		//mouse device lose, try to re-acquire
		dimouse->Acquire();
	}

	//update keyboard
	dikeyboard->Poll();
	if (!SUCCEEDED(dikeyboard->GetDeviceState(256, (LPVOID)&keys)))
	{
		//keyboard device lost, try to re-acquire
		dikeyboard->Acquire();
	}

	//update controllers
	for (int i = 0; i < 3; i++)
	{
		/* error: memset will change the memory which will cover current sprite */
		ZeroMemory(&controllers[i], sizeof(XINPUT_STATE));

		//get the state of the controller
		XINPUT_STATE state;
		DWORD result = XInputGetState(i, &state);

		//store state in global controllers array
		if (result == 0) controllers[i] = state.Gamepad;
	}

}


int Mouse_X()
{
	return mouse_state.lX;
}

int Mouse_Y()
{
	return mouse_state.lY;
}

int Mouse_Button(int button)
{
	return mouse_state.rgbButtons[button] & 0x80;
}

bool Key_Down(int key)
{
	return (bool)(keys[key] & 0x80);
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

bool XInput_Controller_Found()
{
	XINPUT_CAPABILITIES caps;
	ZeroMemory(&caps, sizeof(XINPUT_CAPABILITIES));
	XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps);
	if (caps.Type != 0) return false;

	return true;
}

void XInput_Vibrate(int contNum, int amount)
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = amount;
	vibration.wRightMotorSpeed = amount;
	XInputSetState(contNum, &vibration);
}


LPDIRECT3DTEXTURE9 LoadTexture(string filename, D3DCOLOR transcolor)
{
	LPDIRECT3DTEXTURE9 texture = NULL;

	// get weight and height from bitmap file
	D3DXIMAGE_INFO info;
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result != D3D_OK) return NULL;

	//create the new texture by loading a bitmap image file
	result = D3DXCreateTextureFromFileEx(
		d3ddev, //Direct3D device object
		filename.c_str(), //bitmap filename
		info.Width, // bitmap image width
		info.Height, // bitmap image height
		1, // mip_map levels (1 for no chain)
		D3DPOOL_DEFAULT, // thee type of surface(standard)
		D3DFMT_UNKNOWN, // surface format (default)
		D3DPOOL_DEFAULT, // memory class for the texture
		D3DX_DEFAULT, // image filter
		D3DX_DEFAULT, // mip filter
		transcolor, //color key for transparency
		&info, // bitmap file info (from loaded file)
		NULL, // color palette
		&texture // destination testure
	);
	//make sure the bitmap texture was loaded correctly
	if (result != D3D_OK) return NULL;

	return texture;
}

D3DXVECTOR2 GetBitmapSize(string filename)
{
	D3DXIMAGE_INFO info;
	D3DXVECTOR2 size = D3DXVECTOR2(0.0f, 0.0f);
	HRESULT result = D3DXGetImageInfoFromFile(filename.c_str(), &info);
	if (result == D3D_OK)
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);
	else
		size = D3DXVECTOR2((float)info.Width, (float)info.Height);
	return size;
}


void Sprite_Draw_Frame(LPDIRECT3DTEXTURE9 image, int x, int y,
	int width, int height, int frame, int colums,
	float rotaion, float scaling, D3DCOLOR color)
{
	// create a scale vector
	D3DXVECTOR2 scale(scaling, scaling); 

	// create a translate vector
	D3DXVECTOR2 trans(x, y);

	// set center by dividing width and height by two 
	D3DXVECTOR2 center((float)(width * scaling) / 2, (float)(height * scaling) / 2);

	// create 2D transformation matrix
	D3DXMATRIX mat;
	D3DXMatrixTransformation2D(&mat, NULL, 0, &scale, &center, rotaion, &trans);

	// tell sprite object to use the transform
	spriteobj->SetTransform(&mat);

	// calculate frame location in source image
	int fx = (frame % colums) * width;
	int fy = (frame / colums) * height;
	RECT srcRect = { fx, fy, fx + width, fy + height };

	// draw the sprite frame
	spriteobj->Draw(image, &srcRect, NULL, NULL, color);
}

void Sprite_Animate(int& frame, int startframe, int endframe, 
	int direction, int& starttime, int delay)
{
	if ((int)GetTickCount() > starttime + delay)
	{
		starttime = GetTickCount();
		frame += direction;

		if (frame > endframe) frame = startframe;
		if (frame < startframe) frame = endframe;
	}
}
