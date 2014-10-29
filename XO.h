#pragma once
class XO
{
private:
	int x, y, type;
	unsigned int texture;

public:
	XO(int _x, int _y, int _type, unsigned int _texture);
	int getX();
	int getY();
	int getT();
	unsigned int getTex();
	void setTexture(unsigned int _texture);
	~XO(void);
};

