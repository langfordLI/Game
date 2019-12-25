// MyGame.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "MyGame.h"
#include <time.h>
#include <tchar.h>
#include <mmsystem.h>


#define MAX_LOADSTRING 100
#define WINDOW_WEIGHT 900
#define WINDOW_HEIGHT 700

/************/
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "MSImg32.lib")
/***********/

/*******character**********/
struct Character
{
	int NowHp; // 
	int MaxHp; // life
	int NowMp; // 
	int MaxMp; // magic
	int Level; // 
	int strength; // poweer
	int intelligent;
	int agility;
};

enum ActionTypes
{
	ACTION_TYPE_NORMAL = 0,
	ACTION_TYPE_CRITICAL = 1,
	ACTION_TYPE_MAGIC = 2,
	ACTION_TYPE_MISS = 3,
	ACTION_TYPE_RECOVER = 4
};
/*************************/

/******** global variable **********/
HFONT g_hFont = NULL;
HDC g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;
DWORD g_tPre = 0, g_tNow = 0;
RECT g_rect;
HBITMAP g_hBackground = NULL;
HBITMAP g_hMonster = NULL, g_hHero = NULL;
HBITMAP g_hSkillMonster[4], g_hSkillHero[4], g_hSkillHeroBitmap[4];
Character Hero, Monster;
int g_iFrameNum;
ActionTypes HeroActionType, MonsterActionType;
BOOL g_bCanAttack = false, g_bGameOver = false;
HBITMAP g_hVictory, g_hFailure;
int g_iTextNum;
wchar_t text[8][100];
/*****************/
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
HWND hWnd;
WNDCLASSEXW wcex;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
void Message_Insert(wchar_t* str);

bool Game_Init();
void Game_Paint();
bool Game_CleanUp();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MYGAME, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYGAME));
	PlaySound(L"GameMedia/pSyk - Conga Is Gonna Get You.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
	if (!Game_Init())
	{
		MessageBox(NULL, L"initial fail", L"message window", 0);
		return false;
	}
	
	MSG msg = { 0 };

    // 主消息循环:
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_tNow = GetTickCount();
			if (g_tNow - g_tPre >= 100)
				Game_Paint();
		}
    }
	UnregisterClass(szWindowClass, wcex.hInstance);

    return 0;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
   

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYGAME));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)GetStockObject(GRAY_BRUSH);
    wcex.lpszMenuName   = NULL;// do not need drop-down menu(MAKEINTRESOURCEW(IDC_MYGAME))
	wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm = nullptr; //wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WEIGHT, WINDOW_HEIGHT, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    //case WM_COMMAND:
    //    {
    //        int wmId = LOWORD(wParam);
    //        // 分析菜单选择:
    //        switch (wmId)
    //        {
    //        case IDM_EXIT:
    //            DestroyWindow(hWnd);
    //            break;
    //        default:
    //            return DefWindowProc(hWnd, message, wParam, lParam);
    //        }
    //    }
    //    break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
		break;
    //case WM_PAINT:
    //    {
    //        PAINTSTRUCT ps;
    //        HDC hdc = BeginPaint(hWnd, &ps);
    //        // TODO: 在此处添加使用 hdc 的任何绘图代码...
    //        EndPaint(hWnd, &ps);
    //    }
    //    break;
	case WM_LBUTTONDOWN:
		if (g_bCanAttack == false)
		{
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if (x >= 500 && x <= 550 && y >= 450 && y <= 500)
			{
				g_bCanAttack = true;
				HeroActionType = ACTION_TYPE_NORMAL;
			}
			if (x >= 600 && x <= 650 && y >= 450 && y <= 500)
			{
				g_bCanAttack = true;
				HeroActionType = ACTION_TYPE_MAGIC;
			}
			if (x >= 700 && x <= 750 && y >= 450 && y <= 500)
			{
				g_bCanAttack = true;
				HeroActionType = ACTION_TYPE_RECOVER;
			}
		}
		break;
    case WM_DESTROY:
		Game_CleanUp();
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool Game_Init()
{
	HBITMAP bmp;
	g_hdc = GetDC(hWnd);
	g_mdc = CreateCompatibleDC(g_hdc);
	g_bufdc = CreateCompatibleDC(g_hdc);
	bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WEIGHT, WINDOW_HEIGHT);
	SelectObject(g_mdc, bmp);

	g_hBackground = (HBITMAP)LoadImage(NULL, L"GameMedia/bg.bmp", IMAGE_BITMAP, WINDOW_WEIGHT, WINDOW_HEIGHT, LR_LOADFROMFILE);
	g_hMonster = (HBITMAP)LoadImage(NULL, L"GameMedia/monster.bmp", IMAGE_BITMAP, 400, 400, LR_LOADFROMFILE);
	g_hHero = (HBITMAP)LoadImage(NULL, L"GameMedia/hero.bmp", IMAGE_BITMAP, 360, 360, LR_LOADFROMFILE);
	g_hVictory = (HBITMAP)LoadImage(NULL, L"GameMedia/victory.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
	g_hFailure = (HBITMAP)LoadImage(NULL, L"GameMedia/gameover.bmp", IMAGE_BITMAP, 1086, 396, LR_LOADFROMFILE);
	wchar_t str[100];
	for (int i = 0; i < 4; i++)
	{
		swprintf_s(str, L"GameMedia/skillbutton%d.bmp", i + 1);
		g_hSkillHeroBitmap[i] = (HBITMAP)LoadImage(NULL, str, IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);
	}
	for (int i = 0; i < 4; i++)
	{
		swprintf_s(str, L"GameMedia/skillhero%d.bmp", i + 1);
		g_hSkillHero[i] = (HBITMAP)LoadImage(NULL, str, IMAGE_BITMAP, 360, 360, LR_LOADFROMFILE);
	}
	for (int i = 0; i < 4; i++)
	{
		swprintf_s(str, L"GameMedia/skillmonster%d.bmp", i + 1);
		g_hSkillMonster[i] = (HBITMAP)LoadImage(NULL, str, IMAGE_BITMAP, 360, 360, LR_LOADFROMFILE);
	}
	/***hero****/
	Hero.NowHp = Hero.MaxHp = 1000;
	Hero.NowMp = Hero.MaxMp = 60;
	Hero.Level = 6;
	Hero.strength = 10;
	Hero.agility = 20;
	Hero.intelligent = 10;
	/**** monster ******/
	Monster.NowHp = Monster.MaxHp = 2000;
	Monster.NowMp = Monster.MaxMp = 0;
	Monster.Level = 10;
	Monster.strength = 10;
	Monster.agility = 10;
	Monster.intelligent = 10;
	/*********/
	g_iTextNum = 0;
	g_hFont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, L"Times New Roman");



	GetClientRect(hWnd, &g_rect);
	Game_Paint();
	return true;
}

void Game_Paint()
{
	wchar_t str[100];

	SelectObject(g_bufdc, g_hBackground);
	BitBlt(g_mdc, 0, 0, WINDOW_WEIGHT, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	/**** monster bitmap ******/
	SelectObject(g_bufdc, g_hMonster);
	TransparentBlt(g_mdc, 0, 50, 400, 400, g_bufdc, 0, 0, 400, 400, RGB(0, 0, 0));

	/*** hero bitmap ***/
	SelectObject(g_bufdc, g_hHero);
	TransparentBlt(g_mdc, 500, 50, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
	for (int i = 0; i < 4; i++)
	{
		SelectObject(g_bufdc, g_hSkillHeroBitmap[i]);
		BitBlt(g_mdc, 500 + i * 100, 450, 50, 50, g_bufdc, 0, 0, SRCCOPY);
	}



	/* attack bitmap */
	if (g_bGameOver)
	{
		if (Hero.NowHp <= 0)
		{
			SelectObject(g_bufdc, g_hFailure);
			BitBlt(g_mdc, 178, 102, 543, 396, g_bufdc, 543, 0, SRCAND);
			BitBlt(g_mdc, 178, 102, 543, 396, g_bufdc, 0, 0, SRCPAINT);
		}
		else
		{
			SelectObject(g_bufdc, g_hVictory);
			TransparentBlt(g_mdc, 0, 0, 800, 600, g_bufdc, 0, 0, 800, 600, RGB(0, 0, 0));
		}
	}
	else
	{
		if (g_bCanAttack == true)
		{
			g_iFrameNum++;
			/* hero attack time */
			if (g_iFrameNum >= 5 && g_iFrameNum <= 10)
			{
				int damage = 0;
				/* when at frame 5 calculate currnt damage*/
				if (g_iFrameNum == 5)
				{
					switch (HeroActionType)
					{
					case ACTION_TYPE_NORMAL:
						if (rand() % 4 == 1) // there is 25% rate to trigger big move, 4.5 times damage
						{
							HeroActionType = ACTION_TYPE_CRITICAL;
							/* damag connect with agility, level and strength */
							damage = (int)(4.5f * (float)(3 * (rand() % Hero.agility) + Hero.Level * Hero.strength + 20));
							Monster.NowHp -= damage;
							swprintf_s(str, L"crit %d", damage);
							Message_Insert(str);
						}
						else
						{
							damage = (int)(3 * (rand() % Hero.agility) + Hero.Level * Hero.strength + 20);
							Monster.NowHp -= damage;
							swprintf_s(str, L"hero normal hurt %d", damage);
							Message_Insert(str);
						}
						break;
					case ACTION_TYPE_MAGIC:
						if (Hero.NowMp >= 30)
						{
							/* damag connect with agility, level and intelligence */
							damage = (int)(5 * (2 * rand() % Hero.agility + Hero.Level * Hero.intelligent));
							Hero.NowMp -= 30;
							Monster.NowHp -= damage;
							swprintf_s(str, L"hero magic hurt %d", damage);
							Message_Insert(str);
						}
						else
						{
							HeroActionType = ACTION_TYPE_MISS;
							swprintf_s(str, L"hero no blue");
							Message_Insert(str);
						}
						break;
					case ACTION_TYPE_RECOVER:
						if (Hero.NowMp >= 40)
						{
							/* related to intelligent */
							Hero.NowMp -= 40;
							int recover = 0;
							recover = (int)(5 * (5 * (rand() % Hero.intelligent) + 40));
							Hero.NowHp += recover;
							if (Hero.NowHp >= Hero.MaxHp)
								Hero.NowHp = Hero.MaxHp;
							swprintf_s(str, L"hero recover %d", recover);
							Message_Insert(str);
						}
						else
						{
							HeroActionType = ACTION_TYPE_MISS;
							swprintf_s(str, L"hero no blue");
							Message_Insert(str);
						}
						break;
					}
				}
				switch (HeroActionType)
				{
				case ACTION_TYPE_NORMAL:
					SelectObject(g_bufdc, g_hSkillHero[0]);
					TransparentBlt(g_mdc, 40, 90, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
					break;
				case ACTION_TYPE_MAGIC:
					SelectObject(g_bufdc, g_hSkillHero[1]);
					TransparentBlt(g_mdc, 40, 90, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
					break;
				case ACTION_TYPE_RECOVER:
					SelectObject(g_bufdc, g_hSkillHero[2]);
					TransparentBlt(g_mdc, 500, 50, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
					break;
				case ACTION_TYPE_CRITICAL:
					SelectObject(g_bufdc, g_hSkillHero[3]);
					TransparentBlt(g_mdc, 40, 90, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
					break;
				}
			}
			if (g_iFrameNum == 13)
			{
				if (Monster.NowHp > (Monster.MaxHp / 2))
				{
					switch (rand() % 3)
					{
					case 0:
						MonsterActionType = ACTION_TYPE_NORMAL;
						break;
					case 1:
						MonsterActionType = ACTION_TYPE_CRITICAL;
						break;
					case 2:
						MonsterActionType = ACTION_TYPE_MAGIC;
						break;
					}
				}
				else
				{
					switch (rand() % 3)
					{
					case 0:
						MonsterActionType = ACTION_TYPE_MAGIC;
						break;
					case 1:
						MonsterActionType = ACTION_TYPE_CRITICAL;
						break;
					case 2:
						MonsterActionType = ACTION_TYPE_RECOVER;
						break;
					}
				}
			}
			if (!g_bGameOver)
			{
				int MpRecover = 2 * (rand() % Hero.intelligent) + 6;
				Hero.NowMp += MpRecover;
				if (Hero.NowMp > Hero.MaxMp)
				{
					Hero.NowMp = Hero.MaxMp;
				}
			}
			if (g_iFrameNum >= 15 && g_iFrameNum <= 20)
			{
				int damage = 0;
				int recover = 0;
				/* when at frame 5 calculate currnt damage*/
				if (g_iFrameNum == 15)
				{
					switch (MonsterActionType)
					{
					case ACTION_TYPE_NORMAL:
						MonsterActionType = ACTION_TYPE_CRITICAL;
						/* damag connect with agility, level and strength */
						damage = (int)(rand() % Monster.agility + Monster.Level * Monster.strength);
						Hero.NowHp -= damage;
						swprintf_s(str, L"monster damage %d", damage);
						Message_Insert(str);
						break;
					case ACTION_TYPE_MAGIC:
						damage = (int)(2 * (2 * rand() % Monster.agility) + Monster.strength * Monster.intelligent);
						recover = (int)((float)damage * 0.2f);
						Hero.NowHp -= damage;
						Monster.NowHp += recover;
						if (Monster.NowHp >= Monster.MaxHp)
							Monster.NowHp = Monster.MaxHp;
						swprintf_s(str, L"monster magic %d and recover %d", damage, recover);
						Message_Insert(str);
						break;
					case ACTION_TYPE_RECOVER:
						recover = 2 * Monster.intelligent * Monster.intelligent;
						Monster.NowHp += recover;
						if (Monster.NowHp >= Monster.MaxHp)
							Monster.NowHp = Monster.MaxHp;
						swprintf_s(str, L"monster recover %d", recover);
						Message_Insert(str);
						break;
					case ACTION_TYPE_CRITICAL:
						damage = (int)(2 * (rand() % Monster.agility + Monster.Level * Monster.strength));
						Hero.NowHp -= damage;
						swprintf_s(str, L"monster crit %d", damage);
						Message_Insert(str);
						break;
					}
				}
				switch (MonsterActionType)
				{
					/******* monster sit ********/
				case ACTION_TYPE_NORMAL:
					SelectObject(g_bufdc, g_hSkillMonster[0]);
					TransparentBlt(g_mdc, 550, 90, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
					break;
				case ACTION_TYPE_MAGIC:
					SelectObject(g_bufdc, g_hSkillMonster[2]);
					TransparentBlt(g_mdc, 550, 90, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
					break;
				case ACTION_TYPE_RECOVER:
					SelectObject(g_bufdc, g_hSkillMonster[3]);
					TransparentBlt(g_mdc, 40, 50, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
					break;
				case ACTION_TYPE_CRITICAL:
					SelectObject(g_bufdc, g_hSkillMonster[1]);
					TransparentBlt(g_mdc, 550, 90, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
					break;
				}
			}
			if (Hero.NowHp < 0 || Monster.NowHp < 0)
			{
				g_bGameOver = true;

			}
			
			if (g_iFrameNum == 20)
			{
				g_bCanAttack = false;
				g_iFrameNum = 0;
			}

		}
		
	}

	/* font */
	SelectObject(g_mdc, g_hFont);
	SetBkMode(g_mdc, TRANSPARENT);
	SetTextColor(g_mdc, RGB(50, 255, 255));
	swprintf_s(str, L"HP:%d / %d", Monster.NowHp, Monster.MaxHp);
	TextOut(g_mdc, 40, 411, str, wcslen(str));
	swprintf_s(str, L"MP:%d / %d", Monster.NowMp, Monster.MaxMp);
	TextOut(g_mdc, 40, 431, str, wcslen(str));
	swprintf_s(str, L"HP:%d / %d", Hero.NowHp, Hero.MaxHp);
	TextOut(g_mdc, 500, 411, str, wcslen(str));
	swprintf_s(str, L"MP:%d / %d", Hero.NowMp, Hero.MaxMp);
	TextOut(g_mdc, 500, 431, str, wcslen(str));

	
	for (int i = 0; i < g_iTextNum; i++)
	{
		TextOut(g_mdc, 50, 470 + i * 15, text[i], wcslen(text[i]));
	}
	
	

	BitBlt(g_hdc, 0, 0, WINDOW_WEIGHT, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);

	g_tPre = GetTickCount();
}

bool Game_CleanUp()
{
	DeleteObject(g_hBackground);

	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);

	ReleaseDC(hWnd, g_hdc);
	return true;
}


void Message_Insert(wchar_t* str)
{
	if (g_iTextNum < 8)
	{
		swprintf_s(text[g_iTextNum], str);
		g_iTextNum++;
	}
	else
	{
		for (int i = 0; i < g_iTextNum - 1; i++)
		{
			swprintf_s(text[i], text[i + 1]);
		}
		swprintf_s(text[g_iTextNum - 1], str);
	}
}
