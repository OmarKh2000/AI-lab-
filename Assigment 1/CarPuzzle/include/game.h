#include <iostream>
#ifndef game_H
#define game_H
#include <vector>
#include <string>

class game {
	//std::vector<std::vector<char>> board;
	std::string solution="";
	//int time;
	std::string board="";
	double evaulation = DBL_MAX;
	//int counterNodes = 0;
public:
	game();
	game(std::string s);
	double getEvaulation() const;
	std::string getBoard() const;
	std::string getSolution() const;
	//Game(int size, int time,int** board);
	void updateBoard(std::string newBoard);
	void updateSolution(std::string s);
	bool operator<(const game& r) const;
	bool operator==(const game & r) const;
	//int checkEnd();
	//~Game();
	//int getTime() const;
	//Game(const Game& game);
	game(const game &g2);

	//void applyMove(Move & move, int symbol);

	//void applyandPrintMove(Move & move, int symbol);


	//int updatePlayerTime(int index, int time);

	//void applytemp(Move & move, int symbol);

	//void unapplytemp(Move & move, int symbol);

	//bool checkCanMove(int i, int j);

	//bool checkCanShoot(int i, int j);

	//void printMove(Move & move);

	//int getTurn() const;


};

#endif