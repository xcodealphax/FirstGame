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
		if (ite->frame != -1)
		{
			if ((obj.alive == true) && (Collision(*ite, obj) == true))
			{
				obj.alive = false;
				break;
			}
		}
	}
}

void BOMB_BULLETS::Produce(unsigned int Seed, double probability, int width, int height, int axi_y, int max_length)//probability: 0.00-1.00 最多小数点后两位
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
			if (ite->frame == -1)//找到一颗未占用的
			{
				ite->frame = 0;				
				ite->width = width;
				ite->height = height;
				ite->x = SCREENW - width;
				ite->y = axi_y - height / 2;
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

int BOMB_BULLETS::CalWhichState(int max_length, long length,int warning_dist, int states_nums)//根据飞行距离计算状态(更新frame) 状态数目根据动画图所决定的 
{																							  //参数代表： 最大飞行距离 现在飞行距离 距离目的地多远开始进入警告 状态数目
	int resdist = max_length - length;
	if (resdist < warning_dist)
	{
		int xx = warning_dist - resdist;
		double rate = (double)xx / (double)warning_dist;
		rate *= states_nums;
		int state = rate;//向下取整
		if (state >= states_nums)
			state = states_nums - 1;
		return state;
	}
	else
		return 0;
}

void BOMB_BULLETS::UpDataPosition(FLOWER_BULLETS& flower_class)
{
	//const static int acturally_explosion = 30;//实际爆炸范围大小 必须正方形
	//const static int expect_explosion = 60;//期望爆炸范围大小 必须正方形
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		if ((ite->frame>=0)&&(ite->frame<=10))//0-10是正常飞行画面 0为正常状态 1-10为警告状态
		{
			ite->length += ite->Move();

			ite->frame = CalWhichState(target_distance_map[ite - bullets.begin()], ite->length, 300, 11);

			if (IsOutRange(*ite, SCREENW, SCREENH) == true)
				ite->frame = 11;//进入爆炸画面

			if (ite->length > target_distance_map[ite - bullets.begin()])//飞行到最大距离了
			{
				ite->frame = 11;//进入爆炸画面
			}

			if (ite->frame == 11)//进入爆炸阶段，同时调用花型子弹
				flower_class.Produce(ite->x + ite->width / 2, ite->y + ite->height / 2, 10, 40, 8);
		}
		if (ite->frame >= 11) //11-40为爆炸画面 同时改变sprite大小
		{
			int centre_x = ite->x + ite->width / 2;
			int centre_y = ite->y + ite->height / 2;

			ite->x = centre_x - expect_explosion / 2;
			ite->y = centre_y - expect_explosion / 2;
			ite->width = expect_explosion;
			ite->height = expect_explosion;

			static int now_time = GetTickCount();
			Sprite_Animate(ite->frame, 11, 41, 1, now_time, 30);
			if (ite->frame >= 41)
				ite->frame = -1;//结束周期
		}
		else // -1
			;
	}
}

void BOMB_BULLETS::Init(int width, int height)////hide base->init
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		ite->width = width;
		ite->height = height;
		ite->frame = -1;//ite->alive = false;   alive not work
	}
}

void BOMB_BULLETS::Draw(LPDIRECT3DTEXTURE9 bomb_texture, LPDIRECT3DTEXTURE9 warning_texture, LPDIRECT3DTEXTURE9 explosion_texture)//hide base->draw
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		if(ite->frame ==0)//0是正常状态
		{
			D3DCOLOR bulletscolor = D3DCOLOR_ARGB(255, 255, 255, 255);
			Sprite_Transform_Draw(
				bomb_texture,
				(int)(ite->x),
				(int)(ite->y),
				ite->width,
				ite->height,
				0, 1, 0.0f, 1.0f,
				bulletscolor);
		}
		else if ((ite->frame >= 1) && (ite->frame <= 10))//1-10是warning画面
		{
			D3DCOLOR bulletscolor = D3DCOLOR_ARGB(255, 255, 255, 255);			
			Sprite_Transform_Draw(
				warning_texture,
				(int)(ite->x),
				(int)(ite->y),
				ite->width,
				ite->height,
				ite->frame - 1, 5, 0.0f, 1.0f,
				bulletscolor);
		}
		else if ((ite->frame >= 11) && (ite->frame <= 40))// 爆炸画面
		{
			D3DCOLOR bulletscolor = D3DCOLOR_ARGB(255, 255, 255, 255);
			double scale = (double)expect_explosion / (double)acturally_explosion;
			Sprite_Transform_Draw(
				explosion_texture,
				(int)(ite->x),
				(int)(ite->y),
				acturally_explosion,
				acturally_explosion,
				ite->frame - 11, 6, 0.0f, scale,
				bulletscolor);			
		}
	}
}

struct tmp_format
{
	int x;
	int y;
	double v_x;
	double v_y;
	float rotation;
};

void FLOWER_BULLETS::Produce(int centre_x, int centre_y, int radius, int counts, int speed)
{
	vector<tmp_format> data;//记录坐标等
	tmp_format tmp_data;
	double step = 2 * M_PI / counts;
	for (int i = 0; i < counts; ++i)
	{	
		double angle = (i + 1)*step;
		if (angle <= M_PI) 
		{
			double cs = cos(angle);
			double sn = sin(angle);
			tmp_data.v_x = cs*speed;
			tmp_data.v_y = -sn*speed;
			tmp_data.x = cs*radius + centre_x;
			tmp_data.y = centre_y - sn*radius;
			tmp_data.rotation = angle;
		}
		else if ((angle > M_PI) && (angle <= 2 * M_PI))
		{
			double cs = cos(2 * M_PI - angle);
			double sn = -sin(2 * M_PI - angle);
			tmp_data.v_x = cs*speed;
			tmp_data.v_y = -sn*speed;
			tmp_data.x = cs*radius + centre_x;
			tmp_data.y = centre_y - sn*radius;
			tmp_data.rotation = angle;
		}
		data.push_back(tmp_data);
	}

	int i = 0;
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); (ite != bullets.end()) && (i < counts); ++ite)
	{
		if (ite->alive == false)//找counts颗未用的子弹
		{
			ite->alive = true;
			ite->x = data[i].x;
			ite->y = data[i].y;
			ite->velx = data[i].v_x;
			ite->vely = data[i].v_y;
			ite->rotation = data[i].rotation;
			++i;
		}
	}
}

void FLOWER_BULLETS::Draw(LPDIRECT3DTEXTURE9 texture)
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		if (ite->alive == true)
		{
			D3DCOLOR bulletscolor = D3DCOLOR_ARGB(255, 255, 255, 255);
			
			double rr = M_PI - ite->rotation;
			Sprite_Transform_Draw(
				texture,
				(int)(ite->x),
				(int)(ite->y),
				ite->width,
				ite->height,
				0, 1, 
				rr, //顺时针旋转  注意子弹原始是朝向-x轴的
				1.0f,
				bulletscolor);
		}
	}
}

struct Point
{
	int x;
	int y;
	Point() :x(0), y(0){};
	Point(int xx,int yy) :x(xx), y(yy){};
};
void FLOWER_BULLETS::HandleCollision(SPRITE& obj)//旋转的子弹碰撞
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		if (ite->alive == true)
		{
			/*
			理论上需要重写碰撞函数，以旋转矩形是否相交来处理，
			不过好像有点复杂。。。只能将就原始方式近似了。。。。ORZ
			*/
			if (obj.alive == true)
			{
				Point centre(ite->x + ite->width / 2, ite->y + ite->height / 2);//计算矩形中心 以及对角线长度  为了计算旋转后的四个点坐标
				double aix = sqrt(ite->width*ite->width + ite->height*ite->height);//对角线
				double derta_angle=acos(ite->width/aix);//对角线与x轴夹角 弧度
				Point Pot[4];
				
				//再通过四个点与目标（圆形地球）的距离来判断是否有碰撞
				

			}

			/*if ((obj.alive == true) && (Collision(*ite, obj) == true))
			{
				ite->alive = false;
				obj.alive = false;
				break;
			}*/
		}
	}

}