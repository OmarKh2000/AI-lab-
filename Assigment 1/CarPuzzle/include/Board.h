#pragma once
#ifndef _BOARD_H
#define _BOARD_H

#include<iostream>
#include <string>
#include  "Coordinates.h"

using std::string;
using std::cout;
using std::endl;

#define ROW_SIZE 6
#define FREE_SPOT '.'
#define TARGET_LETTER 'X'
#define TARGET_ROW 2
#define START_EDGE 0
#define END_EDGE 5

typedef enum heuristicType { NON, HEURISTIC1, HEURISTIC2 } Heuristic;

class Board {

	char field[ROW_SIZE][ROW_SIZE];

	/* for Game class  */
	int steps;

	/* buildBoard: building the board from a single line of string */
	void buildBoard(string data);

	/* getCoordinates: returns the Coordinates of a vehicle if found,else reutrn (-1,-1),(-1,-1) */
	Coordinates getCoordinates(char letter);

	/* getMovingSpace: returns the space between two corrdinates (in the same line)  */
	Coordinates getMovingSpace(Coordinates currp, Coordinates newp);

	/* canMoveVehicle: returns true if vehicle can be moved to the desired position , false otherwise*/
	bool canMoveVehicle(char letter, Coordinates currp, Coordinates newp);

	/* fillCoordinates: fills the space betwen the Coordinates with the desired letter */
	void fillCoordinates(char letter, Coordinates position);

	/* calculateH1: calculates the heuristic of a given board situation using heuristic 1 */
	double calculateH1();

	/* calculateH2: calculates the heuristic of a given board situation using heuristic 2 */
	double calculateH2(	);

	/* getDistanceFromExist: returns the number of tiles from the target car to the exist */
	int getDistanceFromExist();

	/* getNumberOfBlockingCars: return the number of blocking cars */
	int getNumberOfBlockingCars(bool checkBlocked = false);

	/* vehicleIsBlocked: returns true if vehicle is blocked and can't move , false otherwise */
	bool vehicleIsBlocked(char letter);

public:
	/* constructor */
	Board(string data) {
		this->buildBoard(data);
	}

	/* destructor */
	~Board() {}

	void setSteps(int n);

	/* printBoard: prints the board to the console */
	void printBoard();

	/* moveVehicle: moves the vehicle to the desired position if possible , returns true if succeeded & false otherwise */
	bool moveVehicle(char letter, Coordinates currentPosition, Coordinates newPosition);

	/* calculateG: calculates the cost of reaching a certain board position */
	double calculateG() {
		return steps;
	}

	/* calculateH: calculates the heuristic of a given board situation */
	double calculateH( Heuristic htype);

	/* calculateF: returns the f cost of a game  */
	double calculateF( Heuristic htype) {
		return calculateG() + calculateH( htype);
	}

};

#endif // !_BOARD_H