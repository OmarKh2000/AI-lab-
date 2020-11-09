#include "Coordinates.h"

void Coordinates::updateLineType() {
	//assuming correct information
	if (this->startp.x == this->endp.x) {
		this->type = HORIZONTAL;
	}
	else {
		this->type = VERTICAL;
	}
}

Coordinates::Coordinates(Point startp, Point endp) {
	this->startp = startp;
	this->endp = endp;
	this->updateLineType();
}

bool Coordinates::inTheSameRow(Coordinates& position) {
	if (this->type != HORIZONTAL || this->type != position.type) {
		return false;
	}
	return this->startp.x == position.startp.x;
}

bool Coordinates::inTheSameCol(Coordinates& position) {
	if (this->type != VERTICAL || this->type != position.type) {
		return false;
	}
	return this->startp.y == position.startp.y;
}