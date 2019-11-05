// Beginning Game Programming, Third Edition
// MyGame.cpp

#include "MyDirectX.h"
#include <exception>
using namespace std;

const string APPTITLE = "DirectX Project <Rename Me>";
const int SCREENW = 1024;
const int SCREENH = 768;

LPDIRECT3DTEXTURE9 explosion = NULL;
int frame1 = 0;
int starttime1;

int frame2 = 10;
int starttime2;


bool Game_Init(HWND window)
{
	//initialize Direct3D
	if (!Direct3D_Init(window, SCREENW, SCREENH, false))
	{
		MessageBox(window, "ERROR initializing Direct3D", "ERROR", 0);
		return false;
	}

	//initialize DirectInput
	if (!DirectInput_Init(window))
	{
		MessageBox(0, "Error initializing DirectInput", "ERROR", 0);
		return false;
	}

	// load non-transparent image
	explosion = LoadTexture("explosion_30_128.tga");
	if (!explosion) return false;

	return true;
}

void Game_Run(HWND window)
{
	//make sure the Direct3D device is valid
	if (!d3ddev) return;

	//update input devices
	DirectInput_Update();

	//clear the scene
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 100), 1.0f, 0);


	//start rendering
	if (d3ddev->BeginScene())
	{
		
		spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		
			Sprite_Animate(frame1, 0, 29, 1, starttime1, 30);
			Sprite_Draw_Frame(explosion, 120, 120, frame1, 128, 128, 6);
		
		

		// stop  drawing

		spriteobj->End();

		//stop rendering
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	//F1 key ends program
	if (Key_Down(DIK_F1))
		gameover = true;

	//controller Back button also ends
	if (controllers[0].wButtons & XINPUT_GAMEPAD_BACK)
		gameover = true;

}

void Game_End()
{
	// free memory and shut down
	explosion->Release();


	//free memory and shut down

	DirectInput_Shutdown();
	Direct3D_Shutdown();
}

