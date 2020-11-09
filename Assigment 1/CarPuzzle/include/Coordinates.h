#pragma once
#ifndef _COORDINATES_H
#define _COORDINATES_H

//blabla
class Point {
public:
	int x;
	int y;

	/* constructor */
	Point(int x = -1, int y = -1) {
		this->x = x;
		this->y = y;
	}

};

typedef enum line_type { VERTICAL, HORIZONTAL } LineType;

class Coordinates {
	Point startp;
	Point endp;
	LineType type;

	/* updateLineType: updates the line type to vertical or horizontal depending on the the current start and end points*/
	void updateLineType();

public:

	/* constructor */
	Coordinates(Point startp = Point(), Point endp = Point());

	/* getLineType: returns the line type */
	LineType getLineType() {
		return this->type;
	}

	/* get start & end points */
	Point& getStartPoint() {
		return this->startp;
	}
	Point& getEndPoint() {
		return this->endp;
	}

	/* set start & end points */
	void setStartPoint(Point& startPoint) {
		this->startp = startPoint;
	}
	void setEndPoint(Point& endPoint) {
		this->endp = endPoint;
	}

	/* inTheSameRow: returns true if this coardinates are in the same row as position , false otherwise */
	bool inTheSameRow(Coordinates& position);

	/* inTheSameCol: returns true if this coardinates are in the same column as position , false otherwise */
	bool inTheSameCol(Coordinates& position);
};

#endif //! _COORDINATES_H