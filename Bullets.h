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

class NORMAL_BULLETS :public CBULLETS  //正常的随机子弹
{
private:
	unsigned long LastTime;
	unsigned long ProduceFrequence;//产生子弹的频率数  单位ms
public:
	NORMAL_BULLETS(int Count) :ProduceFrequence(300), LastTime(GetTickCount()), CBULLETS(Count){ }
	void ProduceOneBullets();
};

class FLOWER_BULLETS :public CBULLETS  //花圈型子弹
{
public:
	virtual void HandleCollision(SPRITE& obj);
	void Produce(int x, int y, int radius, int counts, int speed);
	void Draw(LPDIRECT3DTEXTURE9 texture);
	FLOWER_BULLETS(int Count) : CBULLETS(Count){};
};

class BOMB_BULLETS :public CBULLETS  //炸弹
{
private:
	const static int acturally_explosion = 128;//实际爆炸范围大小 必须正方形
	const static int expect_explosion = 200;//期望爆炸范围大小 必须正方形
	bool ProduceLocker;
	map<int, long> target_distance_map;//最大飞行距离  (index,length)
	int CalWhichState(int max_length,long length,int warning_dist,int states_nums);//根据飞行距离计算状态(更新frame)
public:
	virtual void HandleCollision(SPRITE& obj);
	virtual void UpDataPosition(FLOWER_BULLETS& flower_class);//更新位置与frame  并且调用花型子弹
	void Produce(unsigned int Seed, double probability, int width, int height, int axi_y, int max_length);//probability: 0.00-1.00 最多小数点后两位
	void Draw(LPDIRECT3DTEXTURE9 bomb_texture, LPDIRECT3DTEXTURE9 warning_texture, LPDIRECT3DTEXTURE9 explosion_texture);//hide base->draw
	void Init(int width, int height);////hide base->init
	BOMB_BULLETS(int Count) : CBULLETS(Count){};
};

inline bool CBULLETS::IsOutRange(SPRITE_PLUS& obj, int screen_width, int screen_heigh)//内联始终可见
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