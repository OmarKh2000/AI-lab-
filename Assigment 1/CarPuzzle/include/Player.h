


#include <iostream>
#include "game.h"
#include "Board.h"
#include "ProblemSummary.h"
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_map>
#include <iostream>
#include <cstdio>
#include <ctime>
#include <list>

#ifndef player
#define player



class Player  {
	//std::unordered_map<std::vector<int>, double> map;
	//std::unordered_map<std::vector<int>, int, container_hash<std::vector<int>>> map;
	//std::list<game> open,closed;
	std::map<game,double> open, closed;
	long long int hashCounter = 0;
	long long int nodesExpandexCounter = 0;
	long long int counter = 0;
	long long int maxDepth = 0;
	long long int heuiresticAvg = 0 ;
	long long int avgDepth=0;
	long long int totalDepth = 0;
	Heuristic hType;
public:
	std::vector<game> findMoves(game & g);
	game checkMove(game & g, std::string move);
	Player(Heuristic huType);
	bool checkIsEnd(game & g);
	bool play(game& g, ProblemSummary& summary);
	void ereaseAll();
	double evaluate(game& g);

	//std::vector<Move> findMoves(game & game);

	//void findMovesByIndex(Move & move, std::vector<Move>& moves, Game & game);

	//void findShoots(Move & move, std::vector<Move>& moves, Game & game);

	//double evaulate(Game & game);

	//int evaulateIndex(int i, int j, Game & game);







};

#endif


