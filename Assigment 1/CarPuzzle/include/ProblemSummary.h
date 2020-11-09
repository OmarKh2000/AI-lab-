#pragma once
#ifndef _PROBLEM_SUMMARY_H
#define _PROBLEM_SUMMARY_H
#include "Board.h"

#include <iostream>
#include <string>

using std::string;
using std::ostream;
using std::endl;

#define HEURISTIC_NON_NAME "Non"
#define HEURISTIC_1_NAME "Heuristic 1"
#define HEURISTIC_2_NAME "Heuristic 2"
#define SPACE " "
#define SOLVED_WORD "Yes"
#define NOT_SOLVED_WORD "No"

//typedef enum heuristicType { NON, HEURISTIC1, HEURISTIC2, ALL } Heuristic;

class ProblemSummary {
public:
	int problemNumber;
	Heuristic htype;
	int N; // number of over all nodes
	double penetrationRate; // d/N
	bool solved;
	double time;//in milli-seconds
	double ebf; //effective branching factor
	double avgH; // average H value
	int minDepth;
	double avgDepth;
	int maxDepth;

	/* getHeuristicName: returns the heuristic name */
	string getHeuristicName();

	/* getSolvedInWords: returns a word describing if the problem is solved or not */
	string getSolvedInWords();

	ProblemSummary();
	/* constructor */
	ProblemSummary(int problemNumber, Heuristic heuristic, int N, double penetrationRate, bool solved, double time, double ebf,
		double avgH, int minDepth, double avgDepth, int maxDepth);

	/* printSummary: prints the problem summary to the output */
	void printSummary(ostream& output);


	/* getters */
	int getProblemNumber() {
		return this->problemNumber;
	}

	Heuristic getHeuristic() {
		return this->htype;
	}

	int getN() {
		return this->N;
	}

	double getPenetrationRate() {
		return this->penetrationRate;
	}

	bool getSolved() {
		return this->solved;
	}

	double getTime() {
		return this->time;
	}

	double getEbf() {
		return this->ebf;
	}

	double getAvgH() {
		return this->avgH;
	}

	int getMinDepth() {
		return this->minDepth;
	}

	double getAvgDepth() {
		return this->avgDepth;
	}

	int getMaxDepth() {
		return this->maxDepth;
	}
};

#endif // !_PROBLEM_SUMMARY_H