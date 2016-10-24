#include"Bullets.h"
using namespace std;

CBULLETS::CBULLETS(int Count)
{	
	for (int i = 0; i < Count; ++i)
	{
		SPRITE_PLUS* bullets_ptr = new SPRITE_PLUS;
		bullets.push_back(*bullets_ptr);
	}
};


void CBULLETS::Init(int width, int height)
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		ite->width = width;
		ite->height = height;
		ite->alive = false;
	}

}

void CBULLETS::UpDataPosition()
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end();++ite)
	{
		if (ite->alive == true)
		{
			ite->Move();

			if (IsOutRange(*ite,SCREENW,SCREENH)==true)
				ite->alive = false;
		}
	}
}

void CBULLETS::Draw(LPDIRECT3DTEXTURE9 texture)
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		if (ite->alive == true)
		{
			D3DCOLOR bulletscolor = D3DCOLOR_ARGB(255, 255, 255, 255);
			Sprite_Transform_Draw(
				texture,
				(int)(ite->x),
				(int)(ite->y),
				ite->width,
				ite->height,
				0, 1, 0.0f, 1.0f,
				bulletscolor);
		}
	}
}

void CBULLETS::HandleCollision(SPRITE& obj)
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		if (ite->alive == true)
		{
			if ((obj.alive == true) && (Collision(*ite, obj) == true))
			{
				ite->alive = false;
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
		for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
		{
			if (ite->alive == false)//找到一颗未占用的子弹
			{
				ite->alive = true;
				ite->x = SCREENW - ite->width;
				ite->y = rand() % (SCREENH - ite->height);
				ite->velx = -4.0;
				ite->vely = 0;
				break;
			}
		}
		LastTime = Timer;
	}
}

void BOMB_BULLETS::HandleCollision(SPRITE& obj)
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		if (ite->state != -1)
		{
			if ((obj.alive == true) && (Collision(*ite, obj) == true))
			{
				obj.alive = false;
				break;
			}
		}
	}
}

void BOMB_BULLETS::Produce(unsigned int Seed, double probability, int axi_y, int max_length)//probability: 0.00-1.00 最多小数点后两位
{
	if (ProduceLocker == false)
	{
		srand(Seed);
		ProduceLocker = true;
	}

	int Number = (int)(probability * 100);
	if ((rand() % 100) < Number)//bomb start
	{
		for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
		{
			if (ite->state == -1)//找到一颗未占用的
			{
				ite->alive = 0;
				ite->x = SCREENW - ite->width;
				ite->y = axi_y - ite->height / 2;
				ite->velx = -2.0;
				ite->vely = 0;
				ite->length = 0;
				int index_tmp = ite - bullets.begin();
				target_distance_map[index_tmp] = max_length;
				break;
			} 
		}
	}
}

void BOMB_BULLETS::UpDataPosition()
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		if (ite->state != -1)
		{
			ite->length += ite->Move();

			if (IsOutRange(*ite, SCREENW, SCREENH) == true)
				ite->state = -1;

			if (ite->length > target_distance_map[ite - bullets.begin()])//飞行到最大距离了
			{
				ite->state = -1;
			}
		}
	}
}

void BOMB_BULLETS::Init(int width, int height)////hide base->init
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		ite->width = width;
		ite->height = height;
		ite->state = -1;//ite->alive = false;   alive not work
	}
}