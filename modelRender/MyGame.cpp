// Beginning Game Programming, Third Edition
// MyGame.cpp

#include "MyDirectX.h"
using namespace std;

const string APPTITLE = "Vertex Buffer TExtured Cube";
const int SCREENW = 1024;
const int SCREENH = 768;

MODEL* mesh = NULL;

DWORD screentimer = timeGetTime();

bool Game_Init(HWND window)
{
	srand(time(NULL));

	//initialize Direct3D
	Direct3D_Init(window, SCREENW, SCREENH, false);

	//initialize DirectInput
	DirectInput_Init(window);


	//// create pointer to the back buffer
	//d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &backbuffer);

	// positon the camera
	SetCamera(0.0f, -800.0f, -200.0f);

	// turn dynamic lighting off, z-buffering on
	d3ddev->SetRenderState(D3DRS_LIGHTING, false);
	d3ddev->SetRenderState(D3DRS_ZENABLE, true);

	// load the mesh filee
	mesh = LoadModel("Fokker.X");
	if (mesh == NULL)
	{
		MessageBox(window, "Error loading mesh", APPTITLE.c_str(), MB_OK);
		return 0;
	}
	
	

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

	// slow rendering to approximately 60 fps
	if (timeGetTime() > screentimer + 14)
	{
		screentimer = GetTickCount();

		// start rendering
		if (d3ddev->BeginScene())
		{
			// rotate redering
			D3DXMATRIX matWorld;
			D3DXMatrixRotationY(&matWorld, timeGetTime() / 1000.0f);
			d3ddev->SetTransform(D3DTS_WORLD, &matWorld);
			
			// draw the model
			DrawModel(mesh);

			//stop rendering
			d3ddev->EndScene();
			d3ddev->Present(NULL, NULL, NULL, NULL);
		}
	}


	//exit when escape key is pressed
	if (KEY_DOWN(VK_ESCAPE)) gameover = true;

	//controller Back button also ends
	if (controllers[0].wButtons & XINPUT_GAMEPAD_BACK)
		gameover = true;

}

void Game_End()
{
	// free memory and shut down
	DeleteModel(mesh);
	
	DirectInput_Shutdown();
	Direct3D_Shutdown();
}