#ifndef _MYGAMEH
#define _MYGAMEH
#include "MyDirectX.h"
#include"Layser.h"
#include"Bullets.h"
using namespace std;

bool BackGroundCreat();
void Update_Background();
void Draw_Background();

void Draw_Earth();
void UpdataPosition();

void DrawBoom();

bool Game_Init(HWND window);
void Game_Run(HWND window);
void Game_End();
#endif