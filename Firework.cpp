#include "Firework.h"


Firework::Firework(int _x, int _y, int _width, int _height, int _creationTime, int _totalduration, int _type)
{
	x = _x;
	y = _y;
	Tx = 0;
	Ty = 1;
	stepx = 1.0f / float(_width);
	stepy = 1.0f / float(_height);
	lastTime = _creationTime;
	totalDuration = _totalduration;
	//milisecondPerFrame = 10;
		
	milisecondPerFrame = (50.0f * _totalduration) / float(_width * _height); // 1000 * seconds / (frames_width * frames_height)
	type = _type;
	finished = false;
}

void Firework::Update(int GameTime)
{
	if(GameTime - lastTime >= milisecondPerFrame) 
	{
		if(Tx + stepx < 1.0f) Tx+=stepx;
		else
		{
		  Tx=0;
		  if(Ty - stepx >= 0.0f) Ty-=stepy;
		  else { finished = 1; Ty=1; }
		}
		lastTime = GameTime;
	}
}

bool Firework::IsDead()
{
	return (finished==1);
}

float Firework::getX()
{
	return x;
}

float Firework::getY()
{
	return y;
}

float Firework::getTx()
{
	return Tx;
}

float Firework::getTy()
{
	return Ty;
}

float Firework::getSx()
{
	return stepx;
}
float Firework::getSy()
{
	return stepy;
}

int Firework::getType()
{
	return type;
}

Firework::~Firework(void)
{
}
