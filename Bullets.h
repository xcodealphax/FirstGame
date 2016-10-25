#ifndef _BULLETS
#define _BULLETS

#include"MyDirectX.h"
#include"BasicFunc.h"
#include<vector>
#include<map>
#define _USE_MATH_DEFINES
#include<math.h>
using namespace std;

class CBULLETS
{		
protected:
	vector<SPRITE_PLUS> bullets;
	inline bool IsOutRange(SPRITE_PLUS& obj, int screen_width, int screen_heigh);
public:
	virtual void UpDataPosition();
	void Draw(LPDIRECT3DTEXTURE9 texture);
	virtual void HandleCollision(SPRITE& obj);
	virtual void Init(int width, int height);
	CBULLETS(int Count);
	virtual ~CBULLETS(){};
};

class NORMAL_BULLETS :public CBULLETS  //����������ӵ�
{
private:
	unsigned long LastTime;
	unsigned long ProduceFrequence;//�����ӵ���Ƶ����  ��λms
public:
	NORMAL_BULLETS(int Count) :ProduceFrequence(300), LastTime(GetTickCount()), CBULLETS(Count){ }
	void ProduceOneBullets();
};

class FLOWER_BULLETS :public CBULLETS  //��Ȧ���ӵ�
{
public:
	virtual void HandleCollision(SPRITE& obj);
	void Produce(int x, int y, int radius, int counts, int speed);
	void Draw(LPDIRECT3DTEXTURE9 texture);
	FLOWER_BULLETS(int Count) : CBULLETS(Count){};
};

class BOMB_BULLETS :public CBULLETS  //ը��
{
private:
	const static int acturally_explosion = 128;//ʵ�ʱ�ը��Χ��С ����������
	const static int expect_explosion = 200;//������ը��Χ��С ����������
	bool ProduceLocker;
	map<int, long> target_distance_map;//�����о���  (index,length)
	int CalWhichState(int max_length,long length,int warning_dist,int states_nums);//���ݷ��о������״̬(����frame)
public:
	virtual void HandleCollision(SPRITE& obj);
	virtual void UpDataPosition(FLOWER_BULLETS& flower_class);//����λ����frame  ���ҵ��û����ӵ�
	void Produce(unsigned int Seed, double probability, int width, int height, int axi_y, int max_length);//probability: 0.00-1.00 ���С�������λ
	void Draw(LPDIRECT3DTEXTURE9 bomb_texture, LPDIRECT3DTEXTURE9 warning_texture, LPDIRECT3DTEXTURE9 explosion_texture);//hide base->draw
	void Init(int width, int height);////hide base->init
	BOMB_BULLETS(int Count) : CBULLETS(Count){};
};

inline bool CBULLETS::IsOutRange(SPRITE_PLUS& obj, int screen_width, int screen_heigh)//����ʼ�տɼ�
{
	int LU_x = 0;//left up
	int LU_y = 0;
	int RD_x = 0;//right down
	int RD_y = 0;

	LU_x = obj.x;
	LU_y = obj.y;
	RD_x = obj.x + obj.width;
	RD_y = obj.y + obj.height;

	if ((LU_x<0) || (LU_y<0) || (RD_x>screen_width) || (RD_y>screen_heigh))
		return true;
	else
		return false;
}

#endif