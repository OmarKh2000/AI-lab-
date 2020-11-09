// Genetic5conflictSol.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>					// for cout etc.
#include <vector>					// for vector class
#include <string>					// for string class
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()
using namespace std;				// polluting global namespace, but hey...


#define MAX_STEPS		163800		// maximum iterations
#define N 20 //number of queens


typedef struct p_struct {
	int x;
	int y;
}Point;

typedef struct qn_struct {
	Point p;
}Queen;



typedef vector<Queen> Game; //instead of str





bool calculateConflicts(Game& game, vector<int>& result) {
	std::fill(result.begin(), result.end(), -N-1);

	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (game[i].p.x == game[j].p.x || (game[i].p.x == game[j].p.x) || (game[i].p.y == game[j].p.y) || (game[i].p.y == game[j].p.y)
				|| abs(game[i].p.y - game[j].p.y) == abs(game[i].p.x - game[j].p.x) )
				result[i]++;
				result[j]++;
		}
	}
	for (int i = 0; i < result.size(); i++) {
		if (result[i] > 0)
			return false;
	}
	return true;
}
int sumArray(vector<int> arr) {
	int sum = 0;
	for (int i = 0; i < arr.size(); i++) {
		sum += arr[i];
	}
	return sum;
}
void fixgame(Game& game) {
	vector<int> conflicts(N);	// find conflicted queens
	calculateConflicts(game, conflicts);

	vector<int> maxConflictIndcices;	// choose a random queen from the most conflicted ones.
	maxConflictIndcices.push_back(0);
	for (int i = 1; i < N; i++) {
		if (conflicts[maxConflictIndcices[0]] < conflicts[i]) {
			maxConflictIndcices.clear();
			maxConflictIndcices.push_back(i);
		}
		else if (conflicts[maxConflictIndcices[0]] == conflicts[i]) {
			maxConflictIndcices.push_back(i);
		}
	}
	int choosenQueenIndex = maxConflictIndcices[rand() % maxConflictIndcices.size()];


	Queen replacment;
	int oldConflict = conflicts[choosenQueenIndex];
	//int oldConflict = sumArray(conflicts);
	vector<Queen> replacments;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			replacment.p.x = i;
			replacment.p.y = j;
			game[choosenQueenIndex] = replacment;
			calculateConflicts(game, conflicts);
			if (conflicts[choosenQueenIndex] <= oldConflict) {
			//if (sumArray(conflicts) <= oldConflict) {
				replacments.push_back(replacment);
			}
		}
	}


	//game[choosenQueenIndex] = replacments[rand() % replacments.size()];
	replacment = replacments[rand() % replacments.size()];
	game[choosenQueenIndex].p.x = replacment.p.x;
	game[choosenQueenIndex].p.y = replacment.p.y;

}

void position_queens(Game& game) {
	int tmpx, tmpy;
	for (int i = 0; i < N; i++)
	{
		tmpx = rand() % N;
		tmpy = rand() % N;
		Queen qn;
		qn.p.x = tmpx;
		qn.p.y = tmpy;
		game.push_back(qn);
	}

}

int main()
{
	srand(unsigned(time(NULL)));
	clock_t startTimeStamp = clock();
	clock_t prevTimeStamp = startTimeStamp;
	Game game;
	position_queens(game);
	vector<int> result(N);
	for (int i = 0; i < MAX_STEPS; i++) {
		if (calculateConflicts(game, result)) { // it will reach 0 fitness as always because it has no idea what is it
			clock_t elapsedTime = clock();
			elapsedTime = elapsedTime - startTimeStamp;
			cout << "Reached a solution with time: " << elapsedTime << " clock ticks  = " << (((float)elapsedTime) / CLOCKS_PER_SEC) << " seconds" << endl;
			break;
		}
		fixgame(game);

	}



}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
