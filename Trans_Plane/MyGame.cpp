// Beginning Game Programming, Third Edition
// MyGame.cpp

#include "MyDirectX.h"
#include <exception>
using namespace std;

const string APPTITLE = "DirectX Project <Rename Me>";
const int SCREENW = 1024;
const int SCREENH = 768;

LPDIRECT3DTEXTURE9 image_colorkey = NULL;
LPDIRECT3DTEXTURE9 image_alpha = NULL;
LPDIRECT3DTEXTURE9 image_notrans = NULL;

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
	image_notrans = LoadTexture("shuttle_notrans.bmp");
	if (!image_notrans) return false;

	// load color-keyd transparent image
	image_colorkey = LoadTexture("shuttle_colorkey.bmp", D3DCOLOR_XRGB(255, 0, 255));
	if (!image_colorkey) return false;

	// load alpha transparent image
	image_alpha = LoadTexture("shuttle_alpha.tga");
	if (!image_alpha) return false;

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
		////*** insert sprite code here ***
		//try {
			spriteobj->Begin(D3DXSPRITE_ALPHABLEND);
		//}
		//catch (bad_alloc)
		//{
		//	cout << "Bad Allocation...\n";
		//}

		// draw the sprite
		D3DXVECTOR3 pos1(10, 10, 0);
		spriteobj->Draw(image_notrans, NULL, NULL, &pos1, D3DCOLOR_XRGB(255, 255, 255));

		D3DXVECTOR3 pos2(350, 10, 0);
		spriteobj->Draw(image_colorkey, NULL, NULL, &pos2, D3DCOLOR_XRGB(255, 255, 255));

		D3DXVECTOR3 pos3(700, 10, 0);
		spriteobj->Draw(image_alpha, NULL, NULL, &pos3, D3DCOLOR_XRGB(255, 255, 255));

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
	image_notrans->Release();
	image_colorkey->Release();
	image_alpha->Release();


	//free memory and shut down

	DirectInput_Shutdown();
	Direct3D_Shutdown();
}

