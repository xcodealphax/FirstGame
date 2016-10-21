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

LPDIRECT3DTEXTURE9 Explosion_img;

Laser laser;
Laser laser2;

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
	for (int n = 0; n<BULLETS; n++)
	{
		bullets[n].alive = false;
		bullets[n].x = SCREENW;
		bullets[n].y = SCREENH / 2;
		bullets[n].width = 16;
		bullets[n].height = 8;
	}

	//initialize explosion sprite
	Explosion_img = LoadTexture("explosion_30_128.tga");
	if (!Explosion_img) {
		MessageBox(window, "Error loading sprite", "Error", 0);
		return false;
	}

	//initialize laser
	laser.InitLaser("pre_laser.png", 3, 3, "laser2_head_20X45.tga", "laser2_body_20X45X10.tga", "laser2_tail_20X45X12.tga", 20, 45, 800, 1500);
	laser2.InitLaser("pre_laser.png", 3, 3, "laser2_head_20X45.tga", "laser2_body_20X45X10.tga", "laser2_tail_20X45X12.tga", 20, 45, 800, 1500);

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

void ProduceBullets()
{
	const unsigned long ProduceFrequence = 300;//产生子弹的频率数  单位ms
	unsigned long Timer = GetTickCount();
	static unsigned long LastTime = Timer;
	if ((Timer - LastTime) >= ProduceFrequence)
	{
		for (int i = 0; i < BULLETS; ++i)
		{
			if (bullets[i].alive == false)//找到一颗未占用的子弹
			{
				bullets[i].alive = true;
				bullets[i].x = SCREENW - bullets[i].width;
				bullets[i].y = rand()%(SCREENH - bullets[i].height);
				bullets[i].velx = 4.0;
				bullets[i].vely = 0;
				break;
			}
		}

		LastTime = Timer;
	}
}

void UpDataBullets()
{
	for (int i = 0; i < BULLETS; ++i)
	{
		if (bullets[i].alive == true)
			bullets[i].x = bullets[i].x - bullets[i].velx;

		if (bullets[i].x < 0)
			bullets[i].alive = false;
	}
}

void Drawbullets()
{
	for (int i = 0; i < BULLETS; ++i)
	{
		if (bullets[i].alive == true)
		{
			D3DCOLOR bulletscolor = D3DCOLOR_ARGB(255, 255, 255, 255);
			Sprite_Transform_Draw(
				BULLETS_img,
				(int)bullets[i].x,
				(int)bullets[i].y,
				bullets[i].width,
				bullets[i].height,
				0, 1, 0.0f, 1.0f,
				bulletscolor);
		}
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


void HandCollision()
{
	//****子弹碰撞
	for (int i = 0; i < BULLETS; ++i)
	{
		if (bullets[i].alive == true)
		{
			if ((Earth.alive==true)&&(Collision(bullets[i],Earth) == true))
			{
				bullets[i].alive = false;
				Earth.alive = false;
				break;
			}
		}
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

			ProduceBullets();
			UpDataBullets();
			UpdataPosition();
			

			Draw_Background();
			Draw_Earth();
			Drawbullets();

			laser.DrawLaser();
			laser2.DrawLaser();

			HandCollision();//子弹碰撞处理
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

void Laser::InitLaser(const string pre_img, int width, int height, const string laser_head_img, const string laser_body_mig, const string laser_tail_img, int width2, int height2, unsigned long pr_HoldTime, unsigned long in_HoldTim)
{
	pre_laser = LoadTexture(pre_img);
	for (int i = 0; i < CNT; ++i)
	{
		Pot[i].width = width;
		Pot[i].height = height;
	}

	laser_head = LoadTexture(laser_head_img);
	laser_body = LoadTexture(laser_body_mig);
	for (int i = 0; i < CNT_B; ++i)
	{
		LaserB[i].width = width2;
		LaserB[i].height = height2;
	}

	laser_tail = LoadTexture(laser_tail_img);
	for (int i = 0; i < CNT_T; ++i)
	{
		LaserT[i].width = width2;
		LaserT[i].height = height2;
		LaserT[i].frame = i;
	}


	PreHoldTime = pr_HoldTime;
	InHoldTime = in_HoldTim;

	ProduceLocker = false;
	LastTime = GetTickCount();
	State = None;
}


void Laser::UpdataBody()
{
	//////////////////////updata 激光位置
	for (int i = 0; i < CNT_B; ++i)
	{
		if (LaserB[i].alive)
		{
			LaserB[i].x -= laser_speed;
			if (LaserB[i].x < -LaserB[i].width)
				LaserB[i].alive = false;
		}
	}
}

void Laser::DrawBody()
{
	for (int i = 0; i < CNT_B; ++i)
	{
		if (LaserB[i].alive == true)
		{
			if (LaserB[i].direction != -1)
				Sprite_Draw_Frame(laser_body, LaserB[i].x, LaserB[i].y, LaserB[i].frame, LaserB[i].width, LaserB[i].height, 10);
			else
				Sprite_Draw_Frame(laser_head, LaserB[i].x, LaserB[i].y, LaserB[i].frame, LaserB[i].width, LaserB[i].height, 1);
		}
	}
}

void Laser::DrawLaser()
{	
	Timer = GetTickCount();
	if (State == PreLaser)
	{
		for (int x = CNT; x >= 0; --x)
		{

			D3DCOLOR color = D3DCOLOR_ARGB(255, 255, 255, 255);
			Sprite_Transform_Draw(
				pre_laser,
				Pot[x].x,
				Pot[x].y,
				Pot[x].width,
				Pot[x].height,
				0, 1, 0.0f, 1.0f,
				color);

			if ((Earth.alive==true)&&(Collision(Pot[x], Earth) == true))//碰撞到了即可不用画后面的了
				break;

		}

		if ((Timer - LastTime) >= PreHoldTime)
		{
			LastTime = Timer;
			State = InLaser;
		}
	}
	else if (State == InLaser)//******激光本体
	{		

		//////////////////////updata 激光位置
		UpdataBody();

		////////////////////产生激光序列
		bool IsOneAlive = false;
		for (int i = 0; i < CNT_B; ++i)
		{
			if (LaserB[i].alive == true)
			{
				IsOneAlive = true; break;
			}
		}
		if (IsOneAlive)//产生激光body
		{
			int max_x = 0;//记录最大的末尾值
			for (int i = 0; i < CNT_B; ++i)
			{
				if (LaserB[i].alive == true)
				{
					if ((LaserB[i].x + LaserB[i].width) >= max_x)
						max_x = LaserB[i].x + LaserB[i].width;
				}
			}
			if (max_x >= SCREENW)//大于屏幕了，则不画
				;
			else//小于屏幕 新产生N个激光body 一直到屏幕末尾
			{
				int Cnt =(int) ceil((double)(SCREENW - max_x) / (double)(LaserB[0].width));//计算产生多少个body 向上取整
				for (int j = 0; j < Cnt; ++j)
				{
					int i = 0;
					for (i = 0; i < CNT_B; ++i)
					{
						if (LaserB[i].alive == false)
							break;
					}
					LaserB[i].alive = true;
					LaserB[i].x = max_x + j*LaserB[0].width;
					LaserB[i].y = base_line_y;
					LaserB[i].direction = 0;
					LaserB[i].frame = rand() % 10;//由于激光body的图是20*45*10的，因此随便选一帧
				}
			}
		}
		else//产生激光头(direction==-1)
		{
			LaserB[0].alive = true;
			LaserB[0].x = SCREENW;
			LaserB[0].y = base_line_y;
			LaserB[0].direction = -1;
			LaserB[0].frame = 0;
		}

		/////////////////////Draw激光
		DrawBody();

		if ((Timer - LastTime) >= InHoldTime)
		{
			LastTime = Timer;
			State = OffLaser;//should to OffLazer
		}
	}
	else if (State == OffLaser)//******激光尾
	{
		//****更新激光尾位置
		UpdataTail();
		UpdataBody();

		bool IsBodyAlive = false;//是否有激光体存活标志
		bool IsTailAlive = false;//是否有激光体尾存活标志
		for (int i = 0; i < CNT_B; ++i)
		{
			if (LaserB[i].alive == true)
			{
				IsBodyAlive = true;
				break;
			}
		}
		for (int i = 0; i < CNT_T; ++i)
		{
			if (LaserT[i].alive == true)
			{
				IsTailAlive = true;
				break;
			}
		}
		//构建激光尾部
		if (IsTailAlive == true)
		{
			int i = 0;
			for (i = 0; i < CNT_T; ++i)
			{
				if (LaserT[i].alive==false)
					break;
			}
			if (LaserT[CNT_T - 1].alive)
				;//如果最后的激光尾都活着，那么就不继续构建
			else
			{
				int Cnt = (int)ceil((double)(SCREENW - (LaserT[i - 1].x + LaserT[i - 1].width)) / (double)(LaserT[0].width));//计算产生多少个tail 向上取整
				int k = 0;
				for (int j = i; (j < CNT_T) && (k < Cnt); ++j, ++k)//产生tail
				{
					LaserT[j].alive = true;
					LaserT[j].x = LaserT[j - 1].x + LaserT[j - 1].width;
					LaserT[j].y = base_line_y;
				}
			}
		}
		else if (IsBodyAlive==true)
		{		

			//找最后一个激光体位置
			int max_x = 0;
			for (int i = 0; i < CNT_B; ++i)
			{
				if (LaserB[i].alive == true)
				{
					if ((LaserB[i].x + LaserB[i].width) >= max_x)
						max_x = LaserB[i].x + LaserB[i].width;
				}
			}

			int Cnt = (int)ceil((double)(SCREENW - max_x) / (double)(LaserT[0].width));//计算产生多少个tail 向上取整
			int k = 0;
			for (int j = 0; (j < CNT_T) && (k < Cnt); ++j, ++k)//产生tail
			{
				LaserT[j].alive = true;
				if (j == 0)
					LaserT[0].x = max_x;
				else
					LaserT[j].x = LaserT[j - 1].x + LaserT[j - 1].width;
				LaserT[j].y = base_line_y;
			}
		}
		else
			;//激光尾和激光体都无存活

		//////////Draw激光
		DrawBody();
		DrawTail();

		if ((IsBodyAlive == false) && (IsTailAlive==false))
		{
			LastTime = Timer;
			State = None;//should to OffLazer
		}

	}
}

void Laser::UpdataTail()
{
	//////////////////////updata 激光尾位置
	for (int i = 0; i < CNT_T; ++i)
	{
		if (LaserT[i].alive)
		{
			LaserT[i].x -= laser_speed;
			if (LaserT[i].x < -LaserT[i].width)
				LaserT[i].alive = false;
		}
	}
}

void Laser::DrawTail()
{
	for (int i = 0; i < CNT_T; ++i)
	{
		if (LaserT[i].alive == true)
		{
			Sprite_Draw_Frame(laser_tail, LaserT[i].x, LaserT[i].y, LaserT[i].frame, LaserT[i].width, LaserT[i].height, 12);
		}
	}

}

void Laser::ProduceLaser(unsigned int Seed, double probability, int axi_y)//probability: 0.00-1.00 最多小数点后两位
{
	if (ProduceLocker == false)
	{
		srand(Seed);
		ProduceLocker = true;
	}

	int Number = (int)(probability * 100);
	if ((rand() % 100) < Number)//laser start
	{
		for (int i = CNT - 1; i >= 0; --i)
		{
			Pot[i].x = (float)i;
			Pot[i].y = (float)axi_y;
		}
		for (int i = 0; i < CNT_B; ++i)
		{
			LaserB[i].x = SCREENW + LaserB[i].width;
			LaserB[i].y = axi_y;
			LaserB[i].alive = false;
			LaserB[i].direction = 0;
		}
		for (int i = 0; i < CNT_T; ++i)
		{
			LaserT[i].x = SCREENW + LaserB[i].width;
			LaserT[i].y = axi_y;
			LaserT[i].alive = false;
		}
		base_line_y = axi_y;
		State = PreLaser;//important key 
		LastTime = GetTickCount();
	}
}

bool Laser::IsCollision(SPRITE &obj)//是否有激光碰撞
{
	bool A = false;
	for (int i = 0; i < CNT_B; ++i)
	{
		if (Collision(LaserB[i], obj))
		{
			A = true;
			return A;
		}
	}

	for (int i = 0; i < CNT_T; ++i)
	{
		if (Collision(LaserT[i], obj))
		{
			A = true;
			return A;
		}
	}
	return A;
}