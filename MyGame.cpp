// Beginning Game Programming
// MyGame.cpp

#include "MyDirectX.h"
#include "MyGame.h"
using namespace std;

const string APPTITLE = "Game";
const int SCREENW = 1024;
const int SCREENH = 768;

const int BUFFERW = SCREENW * 2;
const int BUFFERH = SCREENH * 2;

LPDIRECT3DSURFACE9 background = NULL;

LPDIRECT3DTEXTURE9 Earth_img;
SPRITE Earth;

DWORD screentimer = timeGetTime();

double scrollx = 0, scrolly = 0;

LPDIRECT3DTEXTURE9 BULLETS_img;
const int BULLETS = 300;
SPRITE bullets[BULLETS];
NORMAL_BULLETS NormalBullets(400);

LPDIRECT3DTEXTURE9 Explosion_img;

Laser laser;
Laser laser2;

LPDIRECT3DTEXTURE9 BOMB_img;
BOMB_BULLETS BombBullets(3);

bool BackGroundCreat()
{
	//load background
	LPDIRECT3DSURFACE9 image = NULL;
	image = LoadSurface("background.bmp");
	if (!image) return false;

	HRESULT result =
		d3ddev->CreateOffscreenPlainSurface(
		BUFFERW,
		BUFFERH,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&background,
		NULL);
	if (result != D3D_OK) return false;

	//copy image to upper left corner of background
	RECT source_rect = { 0, 0, SCREENW, SCREENH };
	RECT dest_ul = { 0, 0, SCREENW, SCREENH };

	d3ddev->StretchRect(
		image,
		&source_rect,
		background,
		&dest_ul,
		D3DTEXF_NONE);

	//copy image to upper right corner of background
	RECT dest_ur = { SCREENW, 0, SCREENW * 2, SCREENH };

	d3ddev->StretchRect(
		image,
		&source_rect,
		background,
		&dest_ur,
		D3DTEXF_NONE);

	//get pointer to the back buffer
	d3ddev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO,
		&backbuffer);

	//remove image
	image->Release();
	return true;
}

bool Game_Init(HWND window)
{
    srand(time(NULL));

    //initialize Direct3D
    if (!Direct3D_Init(window, SCREENW, SCREENH, false))
    {
        MessageBox(window,"Error initializing Direct3D",APPTITLE.c_str(),0);
        return false;
    }

    //initialize DirectInput
    if (!DirectInput_Init(window))
    {
        MessageBox(window,"Error initializing DirectInput",APPTITLE.c_str(),0);
        return false;
    }

    //initialize DirectSound
    if (!DirectSound_Init(window))
    {
        MessageBox(window,"Error initializing DirectSound",APPTITLE.c_str(),0);
        return false;
    }

	if (!BackGroundCreat())
	{
		MessageBox(window, "Error initializing Background", APPTITLE.c_str(), 0);
		return false;
	}

	//initialize Earth
	Earth_img = LoadTexture("earth.png");
	if (!Earth_img) {
		MessageBox(window, "Error loading sprite", "Error", 0);
		return false;
	}
	Earth.alive = true;
	Earth.x = 100;
	Earth.y = 350;
	Earth.width = Earth.height = 64;

	//initialize Bullets
	BULLETS_img = LoadTexture("bullets.tga", D3DCOLOR_XRGB(0,67,171));//指定掩膜颜色
	if (!BULLETS_img) {
		MessageBox(window, "Error loading sprite", "Error", 0);
		return false;
	}
	NormalBullets.Init(16, 8);


	//initialize explosion sprite
	Explosion_img = LoadTexture("explosion_30_128.tga");
	if (!Explosion_img) {
		MessageBox(window, "Error loading sprite", "Error", 0);
		return false;
	}

	//initialize laser
	laser.InitLaser("pre_laser.png", 8, 3, "laser2_head_20X45.tga", "laser2_body_20X45X10.tga", "laser2_tail_20X45X12.tga", 20, 45, 800, 1500);
	laser2.InitLaser("pre_laser.png", 8, 3, "laser2_head_20X45.tga", "laser2_body_20X45X10.tga", "laser2_tail_20X45X12.tga", 20, 45, 800, 1500);

	//initialize bomb sprite
	BOMB_img = LoadTexture("bomb_45X27.tga");
	if (!BOMB_img) {
		MessageBox(window, "Error loading sprite", "Error", 0);
		return false;
	}
	BombBullets.Init(45, 27);

    return true;
}

void Update_Background()
{
	//update background scrolling
	scrollx += 0.15;
	if (scrolly < 0)
		scrolly = BUFFERH - SCREENH;
	if (scrolly > BUFFERH - SCREENH)
		scrolly = 0;
	if (scrollx < 0)
		scrollx = BUFFERW - SCREENW;
	if (scrollx > BUFFERW - SCREENW)
		scrollx = 0;
}


void Draw_Background()
{
	RECT source_rect = {
		(long)scrollx,
		(long)scrolly,
		(long)scrollx + SCREENW,
		(long)scrolly + SCREENH
	};
	RECT dest_rect = { 0, 0, SCREENW, SCREENH };
	d3ddev->StretchRect(background, &source_rect, backbuffer,
		&dest_rect, D3DTEXF_NONE);
}

void Draw_Earth()
{
	//draw earth normally
	if (Earth.alive == true)
	{
		D3DCOLOR earthcolor = D3DCOLOR_ARGB(255, 255, 255, 255);
		Sprite_Transform_Draw(
			Earth_img,
			(int)Earth.x,
			(int)Earth.y,
			Earth.width,
			Earth.height,
			0, 1, 0.0f, 1.0f,
			earthcolor);
	}
}

void UpdataPosition()
{
	static double L_Step = 0;
	static double R_Step = 0;
	static double U_Step = 0;
	static double D_Step = 0;
	const double step = 0.4;
	const double BaseSpeed = 4.0;
	const double MaxStepDeata = 8.0;

	if (Earth.alive == false)return;

	int Cnt = 0;
	if (Key_Down(DIK_UP))
	{
		U_Step += step;
		if (U_Step >= MaxStepDeata)U_Step = MaxStepDeata;

		Earth.y -= (BaseSpeed + U_Step);

		D_Step = 0;
		Cnt|=1;
	}

	if (Key_Down(DIK_DOWN))
	{
		D_Step += step;
		if (D_Step >= MaxStepDeata)D_Step = MaxStepDeata;

		Earth.y += (BaseSpeed + D_Step);

		U_Step = 0;
		Cnt|=2;
	}

	if (Key_Down(DIK_LEFT))
	{
		L_Step += step;
		if (L_Step >= MaxStepDeata)L_Step = MaxStepDeata;

		Earth.x -= (BaseSpeed + L_Step);

		R_Step = 0;
		Cnt|=4;
	}

	if (Key_Down(DIK_RIGHT))
	{
		R_Step += step;
		if (R_Step >= MaxStepDeata)R_Step = MaxStepDeata;

		Earth.x += (BaseSpeed + R_Step);

		L_Step = 0;
		Cnt|=8;
	}

	if (Earth.x < 0)Earth.x = 0; 
	else if (Earth.x>(SCREENW - Earth.width))Earth.x = (SCREENW - Earth.width);

	if (Earth.y < 0)Earth.y = 0;
	else if (Earth.y>(SCREENH - Earth.height))Earth.y = (SCREENH - Earth.height);

	switch (Cnt)
	{
	case 1:{L_Step = 0; R_Step = 0; break; }
	case 2:{R_Step = 0; L_Step = 0; break; }
	case 4:{U_Step = 0; D_Step = 0; break; }
	case 8:{U_Step = 0; D_Step = 0; break; }
	}	
}


void DrawBoom()
{
	static bool IsBoom = false;//只动画一次 不循环
	if ((IsBoom == false) && (Earth.alive == false))//boom~~~~~
	{
		static int frame = 0;
		static int NowTime = GetTickCount();
		float boom_scale = 1.2;//爆炸放缩比例

		int xx = 0, yy = 0;//记录earth中心点
		xx = Earth.x + Earth.width / 2;
		yy = Earth.y + Earth.height / 2;

		int L_x = (xx - 128 * boom_scale / 2);//爆炸中心点需要与earth中心点对齐,找到爆炸左上角点
		int L_y = (yy - 128 * boom_scale / 2);

		Sprite_Animate(frame, 0, 29, 1, NowTime, 30);
		//Sprite_Draw_Frame(Explosion_img, Earth.x, Earth.y, frame, 128, 128, 6);
		Sprite_Transform_Draw(Explosion_img, L_x, L_y, 128, 128, frame, 6, 0, boom_scale, D3DCOLOR_XRGB(255, 255, 255));//由于爆炸是128*128  地球是64*64  故需要放缩

		if (frame == 29)
			IsBoom = true;
	}
}


void Game_Run(HWND window)
{
    if (!d3ddev) return;
    DirectInput_Update();
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(210, 210, 210), 1.0f, 0);

    /*
     * slow rendering to approximately 60 fps
     */
    if (timeGetTime() > screentimer + 14)
    {
        screentimer = GetTickCount();

		Update_Background();
		
		if (laser.IsLaser()==false)
			laser.ProduceLaser((unsigned int)time(NULL), 0.01, (rand() % 50) * 20);
		if (laser2.IsLaser() == false)
			laser2.ProduceLaser((unsigned int)(time(NULL)*2), 0.2, (rand() % 50) * 20);

        //start rendering
        if (d3ddev->BeginScene())
        {
            //*** insert sprite code here ***
			spriteobj->Begin(D3DXSPRITE_ALPHABLEND);

			NormalBullets.ProduceOneBullets();
			NormalBullets.UpDataPosition();
			BombBullets.Produce((unsigned int)(time(NULL) * 4), 0.01, (rand() % 50) * 20, (rand()%102)*10);
			BombBullets.UpDataPosition();
			UpdataPosition();
			

			Draw_Background();
			Draw_Earth();
			NormalBullets.Draw(BULLETS_img);
			BombBullets.Draw(BOMB_img);

			laser.DrawLaser();
			laser2.DrawLaser();

			NormalBullets.HandleCollision(Earth);//子弹碰撞处理
			BombBullets.HandleCollision(Earth);//炸弹碰撞处理
			laser.HandCollision(Earth);//激光碰撞处理
			laser2.HandCollision(Earth);//激光碰撞处理
			DrawBoom();//爆炸动画

            //stop rendering
			spriteobj->End();
            d3ddev->EndScene();
            d3ddev->Present(NULL, NULL, NULL, NULL);
        }
    }

    //exit with escape key or controller Back button
    if (KEY_DOWN(VK_ESCAPE)) gameover = true;
    if (controllers[0].wButtons & XINPUT_GAMEPAD_BACK) gameover = true;
}

void Game_End()
{
    //free memory and shut down

    DirectSound_Shutdown();
    DirectInput_Shutdown();
    Direct3D_Shutdown();
}

