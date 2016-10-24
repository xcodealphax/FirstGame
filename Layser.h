#ifndef _LAYSERH
#define _LAYSERH

#include <math.h>
#include "MyDirectX.h"
#include "BasicFunc.h"
using namespace std;

extern SPRITE Earth;

enum LaserStateType{ None, PreLaser, InLaser, OffLaser };
class Laser
{
private:
	int laser_speed = 100;//******�����ٶ�

	static const int CNT = 128;
	LaserStateType State;//0 prelaser�׶� 1 laser�׶�	
	LPDIRECT3DTEXTURE9 pre_laser;
	SPRITE_PLUS Pot[CNT];
	LPDIRECT3DTEXTURE9 laser_head;
	LPDIRECT3DTEXTURE9 laser_body;
	static const int CNT_B = 200;
	SPRITE_PLUS LaserB[CNT_B];//direction==-1������ͷ
	LPDIRECT3DTEXTURE9 laser_tail;
	static const int CNT_T = 12;
	SPRITE_PLUS LaserT[CNT_T];

	unsigned long Timer;
	unsigned long LastTime;
	unsigned long PreHoldTime;
	unsigned long InHoldTime;
	int base_line_y;
	bool ProduceLocker;

public:
	void InitLaser(const string pre_img, int width, int height, const string laser_head_img, const string laser_body_mig, const string laser_tail_img, int width2, int height2, unsigned long pr_HoldTime, unsigned long in_HoldTime);
	void ProduceLaser(unsigned int Seed, double probability, int axi_y);//probability: 0.00-1.00 ���С�������λ
	void DrawLaser();
	void UpdataBody();
	void DrawBody();
	void UpdataTail();
	void DrawTail();
	bool IsLaser(){ if (State == None) return false; else return true; }//�Ƿ����ڷ���
	bool IsCollision(SPRITE &obj);//�Ƿ�����ײ
	void HandCollision(SPRITE &obj){ if (IsCollision(obj) == true)obj.alive = false; };//��ײ����
};

#endif