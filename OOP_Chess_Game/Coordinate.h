// only for board
#pragma once
class Coordinate{
private:
	int x;
	int y;
public:
	Coordinate();
	Coordinate(int x, int y);

	void setX(int x);
	void setY(int y);
	int getX() const;
	int getY() const;

	bool operator == (const Coordinate& c) const;
	Coordinate& operator=(const Coordinate& c);
	Coordinate& operator+=(const Coordinate& c);
};
