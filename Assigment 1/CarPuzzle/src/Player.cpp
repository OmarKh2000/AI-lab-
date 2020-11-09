#include "Player.h"
#include"game.h"
#include<string>
#include <iostream>  
#include<sstream> 
#include <list>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#define ROWSIZE 6
game popMin(std::map<game, double>& map) {
	game min;
	double currentMin = DBL_MAX;
	for (auto x : map) {
		if (x.second < currentMin) {
			min = x.first;
			currentMin = x.second;
		}
	}
	return min;


}

std::vector<game> Player::findMoves(game& g) {
	std::vector<game> result;
	std::vector<char> chars;
	string board = g.getBoard();
	for (int i = 0; i < board.length(); i++) {
		if (std::find(chars.begin(), chars.end(), board[i]) == chars.end() && board[i] != '.') {
			chars.push_back(board[i]);
		}
	}
	for (int j = 0; j < chars.size(); j++) {
		for (int i = 1; i < ROWSIZE; i++) {
			game newGame = g;
			char a = chars[j];
			//	string temp= new Character((char)a).toString();
			std::string move;
			move.push_back(a);
			move += "L" + std::to_string(i);
			newGame = checkMove(g, move);
			if ((newGame.getSolution()).length() != (g.getSolution()).length()) {
				result.push_back(newGame);
			}
			move.clear();
			move.push_back(a);
			move += "R" + std::to_string(i);
			newGame = checkMove(g, move);
			if ((newGame.getSolution()).length() != (g.getSolution()).length()) {
				result.push_back(newGame);
			}

			move.clear();
			move.push_back(a);
			move += "D" + std::to_string(i);
			newGame = checkMove(g, move);
			if ((newGame.getSolution()).length() != (g.getSolution()).length()) {
				result.push_back(newGame);
			}

			move.clear();
			move.push_back(a);
			move += "U" + std::to_string(i);
			newGame = checkMove(g, move);
			if ((newGame.getSolution()).length() != (g.getSolution()).length()) {
				result.push_back(newGame);
			}

		}
	}

	return result;
}

game Player::checkMove(game& g, std::string move) {
	
	game newGame = g;
	std::string board = g.getBoard();
	if (board.find(move[0]) == string::npos) {
		return newGame;
	} 
	int startIndexX, startIndexY;
	int endIndexX, endIndexY;
	bool firstINDEX=true;
	for (int i = 0; i < ROWSIZE; i++) {
		for (int j = 0; j < ROWSIZE; j++) {
			if (board[i*ROWSIZE + j] == move[0]) {
				if (firstINDEX) {
					startIndexX = i;
					startIndexY = j;
					firstINDEX = false;
				}
				else {
					endIndexX = i;
					endIndexY = j;
				}
			}

		}
	}
	std::string tempBoard ;
	int moveLength = (move[2] - '0');
 	if (startIndexY == endIndexY) {
		if (move[1] == 'U') {
			if (startIndexX - moveLength < 0) {
				return newGame;
			}
			else {
				 tempBoard = board;
				for (int i = 1; i <= moveLength; i++) {
					if (tempBoard[(startIndexX-i)*ROWSIZE+ startIndexY] != '.') {
						return newGame;
					}
					else {
						tempBoard[(startIndexX - i)*ROWSIZE + endIndexY] = move[0];
						tempBoard[(endIndexX - i + 1)*ROWSIZE + endIndexY] = '.';
					}
				}
			}
			newGame.updateBoard(tempBoard);
			newGame.updateSolution(" " + move);
		}
		else if (move[1] == 'D') {
			if (endIndexX + moveLength >= ROWSIZE) {
				return newGame;
			}
			else {
				tempBoard = board;
				for (int i = 1; i <= moveLength; i++) {
					if (tempBoard[(endIndexX+i)*ROWSIZE + endIndexY ] != '.') {
						return newGame;
					}
					else {
						tempBoard[(endIndexX + i)*ROWSIZE + endIndexY] = move[0];
						tempBoard[(startIndexX + i - 1)*ROWSIZE + endIndexY] = '.';
					}
				}
			}
			newGame.updateBoard(tempBoard);
			newGame.updateSolution(" " + move);
		}	
	}


	if (startIndexX == endIndexX ) {
		if (move[1] == 'L') {
			if (startIndexY - moveLength < 0) {
				return newGame;
			}
			else {
				tempBoard = board;
				for (int i = 1; i <= moveLength; i++) {
					if (tempBoard[startIndexX*ROWSIZE + startIndexY - i] != '.') {
						return newGame;
					}
					else {
						tempBoard[startIndexX*ROWSIZE + startIndexY - i] = move[0];
						tempBoard[endIndexX*ROWSIZE + endIndexY - i + 1] = '.';
					}
				}
			}
			newGame.updateBoard(tempBoard);
			newGame.updateSolution(" " + move);
		}
		else if (move[1] == 'R') {
			if (endIndexY + moveLength >= ROWSIZE) {
				return newGame;
			}
			else {
				tempBoard = board;
				for (int i = 1; i <= moveLength; i++) {
					if (tempBoard[endIndexX*ROWSIZE + endIndexY + i] != '.') {
						return newGame;
					}
					else {
						tempBoard[endIndexX*ROWSIZE + endIndexY + i] = move[0];
						tempBoard[startIndexX*ROWSIZE + startIndexY + i - 1] = '.';
					}
				}
			}
			newGame.updateBoard(tempBoard);
			newGame.updateSolution(" " + move);
		}
	}

	return newGame;






}
Player::Player(Heuristic huType)
{
	this->hType = huType;
}
/*Player::player()
{
}*/
bool Player::checkIsEnd(game& g) {
	string board = g.getBoard();
	return (board[2 * ROWSIZE + 5] == 'X');
}




bool Player::play(game & g, ProblemSummary& summary)
{
	std::clock_t start;
	double duration;

	start = std::clock(); // get current time

	std::cout << "game is:" << g.getBoard() << std::endl;
	open.insert({ g,evaluate(g) });	//	step 1
	int k;
	while (!open.empty()) {	//	step 2
		nodesExpandexCounter++;
		game min = popMin(open);
		heuiresticAvg += evaluate(min);
		avgDepth += min.getSolution().length() / 4;
		//std::cout << min.getSolution() << std::endl;
		//if(min.getSolution().// step 3
		if (checkIsEnd(min) ) {	// need to update g
			g = min;
			std::cout << "number of Nodex= " << counter << std::endl;
			//std::cout << " d/N" << g.getSolution().length() / 4 << std::endl;
			std::cout << g.getSolution();
			std::cout << "EBF= " << counter / nodesExpandexCounter << std::endl;
			std::cout << "avg H value" << heuiresticAvg / nodesExpandexCounter << std::endl;
			std::cout << " avg depth" << avgDepth / nodesExpandexCounter << std::endl;
			std::cout << "depth" << g.getSolution().length() / 4 << std::endl;
			ereaseAll();
			duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

			std::cout << "Operation took " << duration << "seconds" << std::endl << std::endl <<std::endl;

			return true;
		}

		open.erase(min);	// step 4
		std::map<game, double>::const_iterator got = closed.find(min);
		bool found = false;
		for (auto x : closed) {
			if (x.first == min) {
				found = true;
			}
		}
		if (!found) {
			closed.insert({ min,evaluate(min) });
		}

		std::vector<game> moves = findMoves(min);	// step 6
		for (int i = 0; i < moves.size(); i++) {
			double evaluation = evaluate(moves[i]);
			std::map<game, double>::iterator it1;// = closed.find(moves[i]);
			std::map<game, double>::iterator it2;
			bool foundInOpen = false, foundInClosed = false;;

			for (auto x : open) {
				if (x.first == moves[i]) {
					if (x.first.getSolution().length() > moves[i].getSolution().length()) {
						open.erase(x.first);
						open.insert({ moves[i], evaluation });
						counter++;
					}
					foundInOpen = true;
				}
			}
			for (auto x : closed) {
				if (x.first == moves[i]) {
					if (x.first.getSolution().length() > moves[i].getSolution().length()) {
						closed.erase(x.first);
						closed.insert({ moves[i], evaluation });
						open.insert({ moves[i], evaluation });
						counter++;
					}
					foundInClosed = true;
				}
			}
			if (!foundInClosed && !foundInOpen) {
				counter++;
				open.insert({ moves[i],evaluation });
			}
 		}
	}
	ereaseAll();

	return false;



}
void Player::ereaseAll() {
	 hashCounter = 0;
	 counter = 0;
	 maxDepth = 0;
	 heuiresticAvg = 0;
	 avgDepth = 0;
	 totalDepth = 0;
	 nodesExpandexCounter = 0;
	open.clear();
	closed.clear();
}
double Player::evaluate(game & g)
{
	Board board(g.getBoard());
	int k= g.getSolution().length() / 4;
	board.setSteps(g.getSolution().length() / 4);

	return board.calculateF(this->hType);
}
