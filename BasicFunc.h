#ifndef BASICFUNC
#define BASICFUNC

#include"MyDirectX.h"
using namespace std;

class SPRITE_PLUS:public SPRITE
{
private:

public:
	long length;//Ŀǰ���ߵľ���
	int state;
	inline double Move();
	SPRITE_PLUS() {}
};

double SPRITE_PLUS::Move()//��֤���������ɼ�
{
	this->x += this->velx;
	this->y += this->vely;
	return (sqrt((this->velx*this->velx) + (this->vely*this->vely)));
}

#endif