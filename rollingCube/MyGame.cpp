// Beginning Game Programming, Third Edition
// MyGame.cpp

#include "MyDirectX.h"
using namespace std;

const string APPTITLE = "Vertex Buffer TExtured Cube";
const int SCREENW = 1024;
const int SCREENH = 768;

DWORD screentimer = timeGetTime();

// vertex and quad definitions
#define D3DFVF_MYVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)
struct VERTEX // a point
{
	float x, y, z;
	float tu, tv;
};
struct QUAD // a surface
{
	VERTEX vertices[4];
	LPDIRECT3DVERTEXBUFFER9 buffer;
	LPDIRECT3DTEXTURE9 texture;
};
VERTEX cube[] = {
	{-1.0f, 1.0f, -1.0f, 0.0f, 0.0f}, // side1
{1.0f, 1.0f, -1.0f, 1.0f, 0.0f},
{-1.0f, -1.0f, -1.0f, 0.0f, 1.0f},
{1.0f, -1.0f, -1.0f, 1.0f, 1.0f},

{-1.0f, 1.0f, 1.0f, 1.0f, 0.0f}, // side2
{-1.0f, -1.0f, 1.0f, 1.0f, 1.0f},
{1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
{1.0f, -1.0f, 1.0f, 0.0f, 1.0f},

{-1.0f, 1.0f, 1.0f, 0.0f, 0.0f}, // side3
{1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
{-1.0f, 1.0f, -1.0f, 0.0f, 1.0f},
{1.0f, 1.0f, -1.0f, 1.0f, 1.0f},

{-1.0f, -1.0f, 1.0f, 0.0f, 0.0f}, // side4
{-1.0f, -1.0f, -1.0f, 1.0f, 0.0f},
{1.0f, -1.0f, 1.0f, 0.0f, 1.0f},
{1.0f, -1.0f, -1.0f, 1.0f, 1.0f},

{1.0f, 1.0f, -1.0f, 0.0f, 0.0f}, // side5
{1.0f, 1.0f, 1.0f, 1.0f, 0.0f},
{1.0f, -1.0f, -1.0f, 0.0f, 1.0f},
{1.0f, -1.0f, 1.0f, 1.0f, 1.0f},

{-1.0f, 1.0f, -1.0f, 1.0f, 0.0f}, // side6
{-1.0f, -1.0f, -1.0f, 1.0f, 1.0f},
{-1.0f, 1.0f, 1.0f, 0.0f, 0.0f},
{-1.0f, -1.0f, 1.0f, 0.0f, 1.0f},

};

QUAD* quads[6]; // cube
D3DXVECTOR3 cameraSource;
D3DXVECTOR3 cameraTarget;

void SetPosition(QUAD* quad, int ivert, float x, float y, float z) // fill a surface four point
{
	quad->vertices[ivert].x = x;
	quad->vertices[ivert].y = y;
	quad->vertices[ivert].z = z;
}

void SetVertex(QUAD* quad, int ivert, float x, float y, float z, float tu, float tv)
{
	SetPosition(quad, ivert, x, y, z);
	quad->vertices[ivert].tu = tu;
	quad->vertices[ivert].tv = tv;
}

VERTEX CreateVertex(float x, float y, float z, float tu, float tv)
{
	VERTEX vertex;
	vertex.x = x;
	vertex.y = y;
	vertex.z = z;
	vertex.tu = tu;
	vertex.tv = tv;
	return vertex;
}

QUAD* CreateQuad(string textureFilename)
{
	QUAD* quad = (QUAD*)malloc(sizeof(QUAD));

	// load the texture
	D3DXCreateTextureFromFile(d3ddev, textureFilename.c_str(), &quad->texture);
	// create the vertex buffer for this quad
	d3ddev->CreateVertexBuffer(
		4 * sizeof(VERTEX),
		0, // method to ask buffer
		D3DFVF_MYVERTEX,
		D3DPOOL_DEFAULT,
		&quad->buffer,
		NULL
	);

	// create the four corners of this dual triangle strip
	quad->vertices[0] = CreateVertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	quad->vertices[1] = CreateVertex(1.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	quad->vertices[2] = CreateVertex(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	quad->vertices[3] = CreateVertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f);

	return quad;
}

void DeleteQuad(QUAD* quad)
{
	if (quad == NULL)
		return;

	// free the vertex buffer
	if (quad->buffer != NULL)
		quad->buffer->Release();

	// free the texture
	if (quad->texture != NULL)
		quad->texture->Release();

	// free the quad
	free(quad);
}

void DrawQuad(QUAD* quad)
{
	// fill the vertex buffer with this quad's vertices
	void* temp = NULL;
	quad->buffer->Lock(0, sizeof(quad->vertices), (void**)&temp, 0);
	memcpy(temp, quad->vertices, sizeof(quad->vertices));
	quad->buffer->Unlock();

	// draw the textured dual triangle strip
	d3ddev->SetTexture(0, quad->texture);
	d3ddev->SetStreamSource(0, quad->buffer, 0, sizeof(VERTEX));
	d3ddev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void SetIdentity()
{
	// set default position, scale, and rotation
	D3DXMATRIX matWorld;
	D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &matWorld);
}
void SetCamera(float x, float y, float z, float lookx, float looky, float lookz)
{
	D3DXMATRIX matView;
	D3DXVECTOR3 updir(0.0f, 1.0f, 0.0f);

	// move the camera
	cameraSource.x = x;
	cameraSource.y = y;
	cameraSource.z = z;

	// point the camera
	cameraTarget.x = lookx;
	cameraTarget.y = looky;
	cameraTarget.z = lookz;

	// set up the camera view matrix
	D3DXMatrixLookAtLH(&matView, &cameraSource, &cameraTarget, &updir);
	d3ddev->SetTransform(D3DTS_VIEW, &matView);
}

void SetPerspective(float filedOfView, float aspectRadio, float nearRange, float farRange)
{
	// set the perspective so things in the distance will look smaller
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, filedOfView, aspectRadio, nearRange, farRange);
	d3ddev->SetTransform(D3DTS_PROJECTION, &matProj);
}

void init_cube()
{
	for (int q = 0; q < 6; q++)
	{
		int i = q * 4; // little shortcut into cube array
		quads[q] = CreateQuad("cube.bmp");;
		// quads[q] = CreateQuad("cube.bmp");
		for (int v = 0; v < 4; v++)
		{
			quads[q]->vertices[v] = CreateVertex(cube[i].x, cube[i].y, cube[i].z, cube[i].tu, cube[i].tv);
			i++;
		}
	}
}


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
	SetCamera(0.0f, 2.0f, -3.0f, 0, 0, 0);

	float ratio = (float)SCREENW / (float)SCREENH;
	SetPerspective(45.0f, ratio, 0.1f, 10000.0f);

	// turn dynamic lighting off, z-buffering on
	d3ddev->SetRenderState(D3DRS_LIGHTING, false);
	d3ddev->SetRenderState(D3DRS_ZENABLE, true);

	// set thee direct3d stream to use the custom vertex
	d3ddev->SetFVF(D3DFVF_MYVERTEX);

	// convert the cube values into quads
	init_cube();

	return true;
}

void rotate_cube()
{
	static float xrot = 0.0f;
	static float yrot = 0.0f;
	static float zrot = 0.0f;

	// rotate the x and y axes
	xrot += 0.05f;
	yrot += 0.05f;
	zrot += 0.05f;

	// create the matrices
	D3DXMATRIX matWorld;
	D3DXMATRIX matTrans;
	D3DXMATRIX matRot;
	// get an identity matrix
	D3DXMatrixTranslation(&matTrans, 0.0f, 0.0f, 0.0f);

	// rotate the cube
	D3DXMatrixRotationYawPitchRoll(&matRot, D3DXToRadian(xrot), D3DXToRadian(yrot), D3DXToRadian(zrot));
	matWorld = matRot * matTrans;

	// compelet the operation
	d3ddev->SetTransform(D3DTS_WORLD, &matWorld);
}

void rebound(SPRITE& sprite1, SPRITE& sprite2)
{
	float centerx1 = sprite1.x + sprite1.width / 2;
	float centery1 = sprite1.y + sprite1.height / 2;

	float centerx2 = sprite2.x + sprite2.width / 2;
	float centery2 = sprite2.y + sprite2.height / 2;

	if (centerx1 < centerx2)
	{
		sprite1.velx = fabs(sprite1.velx) * -1;
	}
	else
	{
		sprite1.velx = fabs(sprite1.velx);
	}

	if (centery1 < centery2)
	{
		sprite1.vely = fabs(sprite1.vely) * -1;
	}
	else
	{
		sprite1.vely = fabs(sprite1.vely);
	}
	sprite1.x += sprite1.velx;
	sprite1.y += sprite1.vely;
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

		rotate_cube();

		// start rendering
		if (d3ddev->BeginScene())
		{
			for (int n = 0; n < 6; n++)
			{
				DrawQuad(quads[n]);
			}

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
	for (int q = 0; q < 6; q++)
	{
		DeleteQuad(quads[q]);
	}
	DirectInput_Shutdown();
	Direct3D_Shutdown();
}