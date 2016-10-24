#ifndef _BULLETS
#define _BULLETS

#include"MyDirectX.h"
#include"BasicFunc.h"
#include<vector>
#include<map>
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


class BOMB_BULLETS :public CBULLETS  //ը��
{
private:
	bool ProduceLocker;
	map<int, long> target_distance_map;//�����о���  (index,length)
public:
	virtual void HandleCollision(SPRITE& obj);
	virtual void UpDataPosition();
	void Produce(unsigned int Seed, double probability, int axi_y, int max_length);//probability: 0.00-1.00 ���С�������λ
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