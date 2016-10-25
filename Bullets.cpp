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
			if (ite->alive == false)//�ҵ�һ��δռ�õ��ӵ�
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

void BOMB_BULLETS::Produce(unsigned int Seed, double probability, int width, int height, int axi_y, int max_length)//probability: 0.00-1.00 ���С�������λ
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
			if (ite->frame == -1)//�ҵ�һ��δռ�õ�
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

int BOMB_BULLETS::CalWhichState(int max_length, long length,int warning_dist, int states_nums)//���ݷ��о������״̬(����frame) ״̬��Ŀ���ݶ���ͼ�������� 
{																							  //�������� �����о��� ���ڷ��о��� ����Ŀ�ĵض�Զ��ʼ���뾯�� ״̬��Ŀ
	int resdist = max_length - length;
	if (resdist < warning_dist)
	{
		int xx = warning_dist - resdist;
		double rate = (double)xx / (double)warning_dist;
		rate *= states_nums;
		int state = rate;//����ȡ��
		if (state >= states_nums)
			state = states_nums - 1;
		return state;
	}
	else
		return 0;
}

void BOMB_BULLETS::UpDataPosition(FLOWER_BULLETS& flower_class)
{
	//const static int acturally_explosion = 30;//ʵ�ʱ�ը��Χ��С ����������
	//const static int expect_explosion = 60;//������ը��Χ��С ����������
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		if ((ite->frame>=0)&&(ite->frame<=10))//0-10���������л��� 0Ϊ����״̬ 1-10Ϊ����״̬
		{
			ite->length += ite->Move();

			ite->frame = CalWhichState(target_distance_map[ite - bullets.begin()], ite->length, 300, 11);

			if (IsOutRange(*ite, SCREENW, SCREENH) == true)
				ite->frame = 11;//���뱬ը����

			if (ite->length > target_distance_map[ite - bullets.begin()])//���е���������
			{
				ite->frame = 11;//���뱬ը����
			}

			if (ite->frame == 11)//���뱬ը�׶Σ�ͬʱ���û����ӵ�
				flower_class.Produce(ite->x + ite->width / 2, ite->y + ite->height / 2, 10, 40, 8);
		}
		if (ite->frame >= 11) //11-40Ϊ��ը���� ͬʱ�ı�sprite��С
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
				ite->frame = -1;//��������
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
		if(ite->frame ==0)//0������״̬
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
		else if ((ite->frame >= 1) && (ite->frame <= 10))//1-10��warning����
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
		else if ((ite->frame >= 11) && (ite->frame <= 40))// ��ը����
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
	vector<tmp_format> data;//��¼�����
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
		if (ite->alive == false)//��counts��δ�õ��ӵ�
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
				rr, //˳ʱ����ת  ע���ӵ�ԭʼ�ǳ���-x���
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
void FLOWER_BULLETS::HandleCollision(SPRITE& obj)//��ת���ӵ���ײ
{
	for (vector<SPRITE_PLUS>::iterator ite = bullets.begin(); ite != bullets.end(); ++ite)
	{
		if (ite->alive == true)
		{
			/*
			��������Ҫ��д��ײ����������ת�����Ƿ��ཻ������
			���������е㸴�ӡ�����ֻ�ܽ���ԭʼ��ʽ�����ˡ�������ORZ
			*/
			if (obj.alive == true)
			{
				Point centre(ite->x + ite->width / 2, ite->y + ite->height / 2);//����������� �Լ��Խ��߳���  Ϊ�˼�����ת����ĸ�������
				double aix = sqrt(ite->width*ite->width + ite->height*ite->height);//�Խ���
				double derta_angle=acos(ite->width/aix);//�Խ�����x��н� ����
				Point Pot[4];
				
				//��ͨ���ĸ�����Ŀ�꣨Բ�ε��򣩵ľ������ж��Ƿ�����ײ
				

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