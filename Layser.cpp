#include"Layser.h"

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
	//////////////////////updata ����λ��
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
				Sprite_Transform_Draw(laser_body, LaserB[i].x, LaserB[i].y, LaserB[i].width, LaserB[i].height,
				LaserB[i].frame, 10, 0, 1.0f, D3DCOLOR_RGBA(255,255,255,200));
			else
				Sprite_Transform_Draw(laser_body, LaserB[i].x, LaserB[i].y, LaserB[i].width, LaserB[i].height,
				LaserB[i].frame, 1, 0, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 200));
		}
	}
}

void Laser::DrawLaser()
{
	Timer = GetTickCount();
	if (State == PreLaser)
	{
		for (int x = CNT - 1; x >= 0; --x)
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

			if ((Earth.alive == true) && (Collision(Pot[x], Earth) == true))//��ײ���˼��ɲ��û��������
				break;

		}

		if ((Timer - LastTime) >= PreHoldTime)
		{
			LastTime = Timer;
			State = InLaser;
		}
	}
	else if (State == InLaser)//******���Ȿ��
	{

		//////////////////////updata ����λ��
		UpdataBody();

		////////////////////������������
		bool IsOneAlive = false;
		for (int i = 0; i < CNT_B; ++i)
		{
			if (LaserB[i].alive == true)
			{
				IsOneAlive = true; break;
			}
		}
		if (IsOneAlive)//��������body
		{
			int max_x = 0;//��¼����ĩβֵ
			for (int i = 0; i < CNT_B; ++i)
			{
				if (LaserB[i].alive == true)
				{
					if ((LaserB[i].x + LaserB[i].width) >= max_x)
						max_x = LaserB[i].x + LaserB[i].width;
				}
			}
			if (max_x >= SCREENW)//������Ļ�ˣ��򲻻�
				;
			else//С����Ļ �²���N������body һֱ����Ļĩβ
			{
				int Cnt = (int)ceil((double)(SCREENW - max_x) / (double)(LaserB[0].width));//����������ٸ�body ����ȡ��
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
					LaserB[i].y = base_line_y - LaserB[i].height / 2.0;
					LaserB[i].direction = 0;
					LaserB[i].frame = rand() % 10;//���ڼ���body��ͼ��20*45*10�ģ�������ѡһ֡
				}
			}
		}
		else//��������ͷ(direction==-1)
		{
			LaserB[0].alive = true;
			LaserB[0].x = SCREENW;
			LaserB[0].y = base_line_y - LaserB[0].height/2.0;
			LaserB[0].direction = -1;
			LaserB[0].frame = 0;
		}

		/////////////////////Draw����
		DrawBody();

		if ((Timer - LastTime) >= InHoldTime)
		{
			LastTime = Timer;
			State = OffLaser;//should to OffLazer
		}
	}
	else if (State == OffLaser)//******����β
	{
		//****���¼���βλ��
		UpdataTail();
		UpdataBody();

		bool IsBodyAlive = false;//�Ƿ��м��������־
		bool IsTailAlive = false;//�Ƿ��м�����β����־
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
		//��������β��
		if (IsTailAlive == true)
		{
			int i = 0;
			for (i = 0; i < CNT_T; ++i)
			{
				if (LaserT[i].alive == false)
					break;
			}
			if (LaserT[CNT_T - 1].alive)
				;//������ļ���β�����ţ���ô�Ͳ���������
			else
			{
				int Cnt = (int)ceil((double)(SCREENW - (LaserT[i - 1].x + LaserT[i - 1].width)) / (double)(LaserT[0].width));//����������ٸ�tail ����ȡ��
				int k = 0;
				for (int j = i; (j < CNT_T) && (k < Cnt); ++j, ++k)//����tail
				{
					LaserT[j].alive = true;
					LaserT[j].x = LaserT[j - 1].x + LaserT[j - 1].width;
					LaserT[j].y = base_line_y - LaserT[j].height/2.0;
				}
			}
		}
		else if (IsBodyAlive == true)
		{

			//�����һ��������λ��
			int max_x = 0;
			for (int i = 0; i < CNT_B; ++i)
			{
				if (LaserB[i].alive == true)
				{
					if ((LaserB[i].x + LaserB[i].width) >= max_x)
						max_x = LaserB[i].x + LaserB[i].width;
				}
			}

			int Cnt = (int)ceil((double)(SCREENW - max_x) / (double)(LaserT[0].width));//����������ٸ�tail ����ȡ��
			int k = 0;
			for (int j = 0; (j < CNT_T) && (k < Cnt); ++j, ++k)//����tail
			{
				LaserT[j].alive = true;
				if (j == 0)
					LaserT[0].x = max_x;
				else
					LaserT[j].x = LaserT[j - 1].x + LaserT[j - 1].width;
				LaserT[j].y = base_line_y - LaserT[j].height/2.0;
			}
		}
		else
			;//����β�ͼ����嶼�޴��

		//////////Draw����
		DrawBody();
		DrawTail();

		if ((IsBodyAlive == false) && (IsTailAlive == false))
		{
			LastTime = Timer;
			State = None;//should to OffLazer
		}

	}
}

void Laser::UpdataTail()
{
	//////////////////////updata ����βλ��
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
			Sprite_Transform_Draw(laser_tail, LaserT[i].x, LaserT[i].y, LaserT[i].width, LaserT[i].height,
				LaserT[i].frame, 12, 0, 1.0f, D3DCOLOR_RGBA(255, 255, 255, 200));
		}
	}

}

void Laser::ProduceLaser(unsigned int Seed, double probability, int axi_y)//probability: 0.00-1.00 ���С�������λ
{
	if (ProduceLocker == false)
	{
		srand(Seed);
		ProduceLocker = true;
	}

	int Number = (int)(probability * 100);
	if ((rand() % 100) < Number)//laser start
	{
		for (int i = 0; i < CNT; ++i)
		{
			Pot[i].x = (float)(i*8);
			Pot[i].y = (float)axi_y - Pot[i].height / 2.0;
		}
		for (int i = 0; i < CNT_B; ++i)
		{
			LaserB[i].x = SCREENW + LaserB[i].width;
			LaserB[i].y = axi_y - LaserB[i].height / 2.0;
			LaserB[i].alive = false;
			LaserB[i].direction = 0;
		}
		for (int i = 0; i < CNT_T; ++i)
		{
			LaserT[i].x = SCREENW + LaserB[i].width;
			LaserT[i].y = axi_y - LaserT[i].height / 2.0;
			LaserT[i].alive = false;
		}
		base_line_y = axi_y;
		State = PreLaser;//important key 
		LastTime = GetTickCount();
	}
}

bool Laser::IsCollision(SPRITE &obj)//�Ƿ��м�����ײ
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