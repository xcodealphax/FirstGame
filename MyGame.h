#include "MyDirectX.h"
using namespace std;

bool BackGroundCreat();
void Update_Background();
void Draw_Background();

void Draw_Earth();
void UpdataPosition();

void ProduceBullets();
void UpDataBullets();
void Drawbullets();

void HandCollision();

bool Game_Init(HWND window);
void Game_Run(HWND window);
void Game_End();

class Laser
{
private:
	static const int CNT = 1024;
	static int State;//0 prelaser阶段 1 laser阶段	
	LPDIRECT3DTEXTURE9 pre_laser;
	SPRITE Pot[CNT];
	unsigned long Timer;	
	static unsigned long LastTime;
	unsigned long HoldTime;
public:
	void InitLaser(const string img, int width, int height, unsigned long hold_time);
	void ProduceLaser(int y);
	void UpdataLaser();
	void DrawLaser();
	bool IsLaser(){ if ((State == 0) || (State == 1)) return true; else return false; }//是否正在发射

};