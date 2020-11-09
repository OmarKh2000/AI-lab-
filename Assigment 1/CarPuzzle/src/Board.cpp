#include "Board.h"

//========================= private methods ==============================

void Board::buildBoard(string data) {
	for (int i = 0; i < ROW_SIZE; i++)
	{
		for (int j = 0; j < ROW_SIZE; j++)
		{
			int position = ROW_SIZE * i + j;
			field[i][j] = data[position];
		}
	}
}

Coordinates Board::getCoordinates(char letter) {
	//search for the letter
	Coordinates tmp;//((-1,-1),(-1,-1))
	bool foundStartPoint = false;
	Point startp;
	for (int i = 0; i < ROW_SIZE; i++)
	{
		for (int j = 0; j < ROW_SIZE; j++)
		{
			if (this->field[i][j] == letter) {
				startp.x = i;
				startp.y = j;
				tmp.setStartPoint(startp);
				foundStartPoint = true;
				break;
			}
		}
		if (foundStartPoint) {
			break;
		}
	}
	if (!foundStartPoint) {
		return tmp;//return the default coordinates
	}
	//found starting point
	LineType type = HORIZONTAL;//default
	if (startp.x < ROW_SIZE - 1) {
		if (this->field[startp.x + 1][startp.y] == letter) {
			//move forward as column
			type = VERTICAL;
		}
	}
	if (startp.y < ROW_SIZE - 1) {
		if (this->field[startp.x][startp.y + 1] == letter) {
			//move forward as row
			type = HORIZONTAL;
		}
	}
	int startIndex = 0;
	int endIndex = 0;
	if (type == HORIZONTAL) {
		endIndex = startp.y;
	}
	else { //VERTICAL
		endIndex = startp.x;
	}
	for (endIndex; endIndex <= ROW_SIZE; endIndex++)
	{
		if (type == VERTICAL) {
			if (this->field[endIndex][startp.y] != letter) {
				break;
			}
		}
		else { // HORIZONTAL
			if (this->field[startp.x][endIndex] != letter) {
				break;
			}
		}
	}
	endIndex--;//get the actual one
	Point endp;
	if (type == VERTICAL) {
		endp.x = endIndex;
		endp.y = startp.y;
	}
	else { // HORIZONTAL
		endp.x = startp.x;
		endp.y = endIndex;
	}
	return Coordinates(startp, endp);
}

Coordinates Board::getMovingSpace(Coordinates currp, Coordinates newp) { //assuming correct info
	Coordinates tmp = currp;
	if (currp.inTheSameRow(newp)) {
		if (currp.getStartPoint().y <= newp.getStartPoint().y) {
			tmp.setStartPoint(currp.getStartPoint());
		}
		else {
			tmp.setStartPoint(newp.getStartPoint());
		}
		if (currp.getEndPoint().y >= newp.getEndPoint().y) {
			tmp.setEndPoint(currp.getEndPoint());
		}
		else {
			tmp.setEndPoint(newp.getEndPoint());
		}
	}
	else { //must be in the same column
		if (currp.getStartPoint().x <= newp.getStartPoint().x) {
			tmp.setStartPoint(currp.getStartPoint());
		}
		else {
			tmp.setStartPoint(newp.getStartPoint());
		}
		if (currp.getEndPoint().x >= newp.getEndPoint().x) {
			tmp.setEndPoint(currp.getEndPoint());
		}
		else {
			tmp.setEndPoint(newp.getEndPoint());
		}
	}
	return tmp;
}


bool Board::canMoveVehicle(char letter, Coordinates currp, Coordinates newp) {
	LineType type = currp.getLineType();
	if (type != newp.getLineType()) { //cannot move from verticle to horizontal
		return false;
	}
	Coordinates movingSpace = this->getMovingSpace(currp, newp);
	int startIndex = 0;
	int endIndex = 0;
	int constIndex = 0;
	if (type == HORIZONTAL) {
		startIndex = movingSpace.getStartPoint().y;
		endIndex = movingSpace.getEndPoint().y;
		constIndex = movingSpace.getStartPoint().x;
	}
	else {
		startIndex = movingSpace.getStartPoint().x;
		endIndex = movingSpace.getEndPoint().x;
		constIndex = movingSpace.getStartPoint().y;
	}
	for (int i = startIndex; i <= endIndex; i++)
	{
		char value = FREE_SPOT;
		if (type == VERTICAL) {
			value = this->field[i][constIndex];
		}
		else { // HORIZONTAL
			value = this->field[constIndex][i];
		}
		if (value != FREE_SPOT && value != letter) {
			return false;
		}
	}
	return true;
}

void Board::fillCoordinates(char letter, Coordinates position) {
	int startIndex = 0;
	int endIndex = 0;
	int constIndex = 0;
	LineType type = position.getLineType();
	if (type == HORIZONTAL) {
		startIndex = position.getStartPoint().y;
		endIndex = position.getEndPoint().y;
		constIndex = position.getStartPoint().x;
	}
	else {
		startIndex = position.getStartPoint().x;
		endIndex = position.getEndPoint().x;
		constIndex = position.getStartPoint().y;
	}
	for (int i = startIndex; i <= endIndex; i++)
	{
		if (type == VERTICAL) {
			this->field[i][constIndex] = letter;
		}
		else { // HORIZONTAL
			this->field[constIndex][i] = letter;
		}
	}
}

int Board::getDistanceFromExist() {
	int distance = 0;
	for (int i = ROW_SIZE - 1; i >= 0; i--)
	{
		distance++;
		if (this->field[TARGET_ROW][i] == TARGET_LETTER) {
			break;
		}
	}
	return distance;
}

int Board::getNumberOfBlockingCars(bool checkBlocked) {
	int num = 0;
	char ch = FREE_SPOT;//default value
	for (int i = ROW_SIZE - 1; i >= 0; i--)
	{
		ch = this->field[TARGET_ROW][i];
		if (ch != FREE_SPOT && ch != TARGET_LETTER) {
			num++;
			if (checkBlocked && this->vehicleIsBlocked(ch)) {
				num++;//all in all 2 points for blocked blocking cars
			}
		}
		if (ch == TARGET_LETTER) {
			break;
		}
	}
	return num;
}

bool Board::vehicleIsBlocked(char letter) {
	Coordinates cord = this->getCoordinates(letter);
	Point startp = cord.getStartPoint();
	Point endp = cord.getEndPoint();
	if (cord.getLineType() == VERTICAL) {
		if (startp.x == START_EDGE || (startp.x > START_EDGE && this->field[startp.x - 1][startp.y] != FREE_SPOT)) {
			//check the bottom-end point
			if (endp.x == END_EDGE || (endp.x < END_EDGE && this->field[endp.x + 1][endp.y] != FREE_SPOT)) {
				return true;
			}
		}
	}
	else {//horizontal
		if (startp.y == START_EDGE || (startp.y > START_EDGE && this->field[startp.x][startp.y - 1] != FREE_SPOT)) {
			//check the bottom-end point
			if (endp.y == END_EDGE || (endp.y < END_EDGE && this->field[endp.x][endp.y + 1] != FREE_SPOT)) {
				return true;
			}
		}
	}
	return false;
}

double Board::calculateH1() {
	//must be in steps
	//the distance from the exist + the number of cars blocking which are blocked
	int distance = this->getDistanceFromExist();
	int blockingCars = this->getNumberOfBlockingCars();
	return distance + blockingCars;
}

double Board::calculateH2() {
	//must be in steps
	//the distance from the exist + the number of cars blocking which are blocked
	int distance = this->getDistanceFromExist();
	int blockedBlockingCars = this->getNumberOfBlockingCars(true);
	return distance + blockedBlockingCars;
}



//========================= public methods ==============================



void Board::setSteps(int n)
{
	steps = n;
}

void Board::printBoard() {
	//make sure to call it when it's full
	cout << "The board looks like: " << endl;
	for (size_t i = 0; i < ROW_SIZE; i++)
	{
		for (size_t j = 0; j < ROW_SIZE; j++)
		{
			int position = ROW_SIZE * i + j;
			cout << field[i][j] << " ";
		}
		cout << endl;
	}
}

bool Board::moveVehicle(char letter, Coordinates currentPosition, Coordinates newPosition) {
	if (!this->canMoveVehicle(letter, currentPosition, newPosition)) {
		return false;
	}
	//do the moving
	//empty the old position
	this->fillCoordinates(FREE_SPOT, currentPosition);
	//fill the new position
	this->fillCoordinates(letter, newPosition);
	return true; //succeeded
}

double Board::calculateH(Heuristic htype) {
	switch (htype) {
	case NON: return 0;
	case HEURISTIC1: return this->calculateH1();
	case HEURISTIC2: return this->calculateH2();
	default:  return 0;
	}
}