#ifndef BASICFUNC
#define BASICFUNC

#include"MyDirectX.h"
using namespace std;

class SPRITE_PLUS:public SPRITE
{
private:

public:
	inline void Move();
	SPRITE_PLUS() {SPRITE();}
};

void SPRITE_PLUS::Move()//��֤���������ɼ�
{
	this->x += this->velx;
	this->y += this->vely;
}

#endif