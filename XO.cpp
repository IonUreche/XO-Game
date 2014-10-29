#include "XO.h"


XO::XO(int _x, int _y, int _type, unsigned int _texture)
{
	x = _x;
	y = _y;
	type = _type;
	texture = _texture;
}

int XO::getX()
{
	return x;
}

int XO::getY()
{
	return y;
}

int XO::getT()
{
	return type;
}

unsigned int XO::getTex()
{
	return texture;
}

void XO::setTexture(unsigned int _texture)
{
	texture = _texture;
}

XO::~XO(void)
{
}
