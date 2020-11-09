#include <string>
#include "game.h"
#define ROWSIZE 6
game::game()
{
}

game::game(std::string s)
{
	/*board.resize(ROWSIZE);
	for(int i = 0; i < ROWSIZE; i++) {
		board[i].resize(ROWSIZE);
	}
	for (int i = 0; i < s.length(); i++) {
		board[i/ROWSIZE][i%ROWSIZE] = s[i];
	}*/
	board = s;
}

double game::getEvaulation() const
{
	return evaulation;
}

std::string game::getBoard() const
{
	return board;
}

std::string game::getSolution() const
{
	return solution;
}

void game::updateBoard(std::string newBoard)
{
	board = newBoard;
}

void game::updateSolution(std::string s)
{
	solution += s;
}

bool game::operator<(const game & r) const
{
	return this->board < r.getBoard();
	//return false;
}

bool game::operator==(const game & r) const
{
	return this->solution == r.getSolution();
}

game::game(const game & g2)
{
	this->board = g2.getBoard();
	this->solution = g2.getSolution();
	this->evaulation = g2.getEvaulation();
}


