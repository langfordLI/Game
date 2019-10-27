#include "MyDirectX.h"
using namespace std;

const string APPTITLE = "Bomb Catcher Game";
const int SCREENW = 1024;
const int SCREENH = 768;

LPDIRECT3DSURFACE9 bomb_surf = NULL;
LPDIRECT3DSURFACE9 bucket_surf = NULL;

struct BOMB
{
	float x, y;
	
	void reset()
	{
		x = (float)(rand() % (SCREENW - 128));
		y = 0;
	}
};
BOMB bomb;
struct BUCKET
{
	float x, y;
};
BUCKET bucket;

int score;
/*
game initialization
*/

bool Game_Init(HWND window)
{
	// initial device
	Direct3D_Init(window, SCREENW, SCREENH, false);
	DirectInput_Init(window);

	// load surface
	bomb_surf = LoadSurface("bomb.bmp");
	if (!bomb_surf)
	{
		MessageBox(window, "Error loading bomb", "Error", 0);
		return false;
	}
	bucket_surf = LoadSurface("bucket.bmp");
	if (!bucket_surf)
	{
		MessageBox(window, "Error loading bucket", "Error", 0);
		return false;
	}


	// position the bomb;
	srand((unsigned int)time(NULL));
	bomb.reset();

	//position the bucket
	bucket.x = 500;
	bucket.y = 630;
	return true;

}
void Game_Run(HWND window)
{
	// make sure the Direct3D devicee is valid
	if (!d3ddev) return;

	// update input device
	DirectInput_Update();

	// move bomb down the screen
	bomb.y += 2.0f;

	// see if bomb hit the floor
	if (bomb.y > SCREENH)
	{
		MessageBox(0, "the bomb is exploded!!", "you lose", 0);
		gameover = true;
	}
	//// move the bucket with the mouse
	//int mx = Mouse_X();
	//if (mx < 0) { bucket.x -= 6.0f; }
	//else if (mx > 0) { bucket.x += 6.0f; }

	//move tehe bucket with the keyboard
	if (Key_Down(DIK_LEFT)) bucket.x -= 6.0f;
	else if (Key_Down(DIK_RIGHT)) bucket.x += 6.0f;

	// keep bucket in the screen
	if (bucket.x < 0) bucket.x = 0;
	if (bucket.x > SCREENW - 128) bucket.x = SCREENW - 128;

	// see if the bucket catch the bomb
	int cx = bomb.x + 64;
	int cy = bomb.y + 64;
	if (cx > bucket.x&& cx < bucket.x + 128 && cy > bucket.y&& cy < bucket.y + 128)
	{
		//update sorce
		score++;
		char s[255];
		sprintf_s(s, "%s[score %d]", APPTITLE.c_str(), score);
		SetWindowText(window, s);

		// reset the bomb
		bomb.reset();
	}

	//clear the backbuffer
	d3ddev->ColorFill(backbuffer, NULL, D3DCOLOR_XRGB(0, 0, 0));

	//start rendering
	if (d3ddev->BeginScene())
	{
		// draw the bomb
		DrawSurface(backbuffer, bomb.x, bomb.y, bomb_surf);
		// draw the bucket
		DrawSurface(backbuffer, bucket.x, bucket.y, bucket_surf);

		// stop rendering
		d3ddev->EndScene();
		d3ddev->Present(NULL, NULL, NULL, NULL);
	}

	if (Key_Down(DIK_ESCAPE))
		gameover = true;
}
void Game_End()
{
	if (bomb_surf) bomb_surf->Release();
	if (bucket_surf) bucket_surf->Release();
	DirectInput_Shutdown();
	Direct3D_Shutdown();
}
