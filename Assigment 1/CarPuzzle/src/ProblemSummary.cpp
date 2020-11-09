#include "ProblemSummary.h"

//============== private methods ===============

string ProblemSummary::getHeuristicName() {
	string name = "";
	switch (this->htype) {
	case NON: name = HEURISTIC_NON_NAME;
		break;
	case HEURISTIC1: name = HEURISTIC_1_NAME;
		break;
	case HEURISTIC2: name = HEURISTIC_2_NAME;
		break;
	default:
		break;;
	}
	return name;
}

string ProblemSummary::getSolvedInWords() {
	if (this->solved) {
		return SOLVED_WORD;
	}
	return NOT_SOLVED_WORD;
}

ProblemSummary::ProblemSummary()
{
}

//============== public methods ===============

ProblemSummary::ProblemSummary(int problemNumber, Heuristic heuristic, int N, double penetrationRate, bool solved, double time, double ebf,
	double avgH, int minDepth, double avgDepth, int maxDepth) {

	this->problemNumber = problemNumber;
	this->htype = heuristic;
	this->N = N;
	this->penetrationRate = penetrationRate;
	this->solved = solved;
	this->time = time;
	this->ebf = ebf;
	this->avgH = avgH;
	this->minDepth = minDepth;
	this->avgDepth = avgDepth;
	this->maxDepth = maxDepth;
}

void ProblemSummary::printSummary(ostream& output) {
	output << this->problemNumber << SPACE << this->getHeuristicName() << SPACE << this->N << SPACE <<
		this->penetrationRate << SPACE << this->getSolvedInWords() << SPACE << this->time << SPACE << this->ebf << SPACE <<
		this->avgH << SPACE << this->minDepth << SPACE << this->avgDepth << SPACE << this->maxDepth << endl;
}