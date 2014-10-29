#pragma once
class Firework
{
private:
	int x, y, lastTime, totalDuration, type;
	float Tx, Ty, stepx, stepy, milisecondPerFrame;
	bool finished;

public:
	Firework(int x, int y, int width, int height, int lasttime, int totalduration, int type);
	void Update(int GameTime);
	bool IsDead();
	float getX();
	float getY();
	float getTx();
	float getTy();
	float getSx();
	float getSy();
	int getType();
	~Firework(void);
};

