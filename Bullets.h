#ifndef _BULLETS
#define _BULLETS

#include"MyDirectX.h"
#include"BasicFunc.h"
using namespace std;

class CBULLETS
{	
protected:
	static const int BulletsCount = 400;
	SPRITE_PLUS bullets[BulletsCount];
public:
	void UpDataBullets();
	void Drawbullets(LPDIRECT3DTEXTURE9 texture);
	virtual void HandleCollision(SPRITE& obj);
};

class NORMAL_BULLETS :public CBULLETS  //����������ӵ�
{
private:
	unsigned long LastTime;
	unsigned long ProduceFrequence;//�����ӵ���Ƶ����  ��λms
public:
	NORMAL_BULLETS() :ProduceFrequence(300), LastTime(GetTickCount()){ CBULLETS(); }
	void ProduceOneBullets();
	void Init(int width,int height);

};




#endif