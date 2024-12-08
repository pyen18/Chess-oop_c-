#include "Coordinate.h"

Coordinate::Coordinate() {
	x = 0;
	y = 0;
}
Coordinate::Coordinate(int x, int y) {
	this->x = x;
	this->y = y;
}

void Coordinate::setX(int x) {
	this->x = x;
}
void Coordinate::setY(int y) {
	this->y = y;
}
int Coordinate::getX() const {
	return this->x;
}
int Coordinate::getY() const {
	return this->y;
}

bool Coordinate::operator == (const Coordinate& c) const {
	if (this->getX() == c.getX() && this->getY() == c.getY()) {
		return true;
	}
	return false;
}

Coordinate& Coordinate::operator=(const Coordinate& c) {
	this->setX(c.getX());
	this->setY(c.getY());

	return *this;
}

Coordinate& Coordinate::operator+=(const Coordinate& c) {
	this->setX(this->getX() + c.getX());
	this->setY(this->getY() + c.getY());

	return *this;
}

