#include"Bullets.h"
using namespace std;

void CBULLETS::UpDataBullets()
{
	for (int i = 0; i < BulletsCount; ++i)
	{
		if (bullets[i].alive == true)
			bullets[i].Move();

		if (bullets[i].x < 0)
			bullets[i].alive = false;
	}
}

void CBULLETS::Drawbullets(LPDIRECT3DTEXTURE9 texture)
{
	for (int i = 0; i < BulletsCount; ++i)
	{
		if (bullets[i].alive == true)
		{
			D3DCOLOR bulletscolor = D3DCOLOR_ARGB(255, 255, 255, 255);
			Sprite_Transform_Draw(
				texture,
				(int)bullets[i].x,
				(int)bullets[i].y,
				bullets[i].width,
				bullets[i].height,
				0, 1, 0.0f, 1.0f,
				bulletscolor);
		}
	}
}

void CBULLETS::HandleCollision(SPRITE& obj)
{
	for (int i = 0; i < BulletsCount; ++i)
	{
		if (bullets[i].alive == true)
		{
			if ((obj.alive == true) && (Collision(bullets[i], obj) == true))
			{
				bullets[i].alive = false;
				obj.alive = false;
				break;
			}
		}
	}

}

void NORMAL_BULLETS::ProduceOneBullets()
{
	unsigned long Timer = GetTickCount();
	if ((Timer - LastTime) >= ProduceFrequence)
	{
		for (int i = 0; i < BulletsCount; ++i)
		{
			if (bullets[i].alive == false)//找到一颗未占用的子弹
			{
				bullets[i].alive = true;
				bullets[i].x = SCREENW - bullets[i].width;
				bullets[i].y = rand() % (SCREENH - bullets[i].height);
				bullets[i].velx = -4.0;
				bullets[i].vely = 0;
				break;
			}
		}
		LastTime = Timer;
	}
}

void NORMAL_BULLETS::Init(int width, int height)
{
	LastTime = GetTickCount();
	for (int n = 0; n<BulletsCount; n++)
	{
		bullets[n].alive = false;
		bullets[n].width = width;
		bullets[n].height = height;
	}
}