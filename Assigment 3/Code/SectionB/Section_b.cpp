
#pragma warning(disable:4786)		// disable debug warning

#include <iostream>					// for cout etc.
#include <vector>					// for vector class
#include <array>					// for array class
#include <string>					// for string class
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()


//======== HW3 ======
#define POP_SIZE 1000
#define GENE_SIZE 20
#define TRIALS_LIMIT 1000
#define MAX_GENERATIONS 100 //how many generations to run
#define NO_CONNECTION  '0'
#define CONNECTION  '1'
#define UNKNOWN '?'

using namespace std;				// polluting global namespace, but hey...

//============ HW3 ======

typedef array<char,GENE_SIZE> HN_gene; //Holten & Nowlan

struct Organism {
	HN_gene gene;
	int fitness;
};

typedef vector<Organism> HN_population;

/* for statics later */
vector<double> avg_correct_pos(MAX_GENERATIONS);
vector<double> avg_incorrect_pos(MAX_GENERATIONS);
vector<double> avg_learned_bits(MAX_GENERATIONS);

struct FitnessPoint {
	int startPos;
	int endPos;
	int length;
};



/* init_target: initialize the target */
void init_target(HN_gene& target) {
	// 0 = '0' , 1='1'
	int rnd = 0;
	char letter;
	for (int i = 0; i < GENE_SIZE; i++)
	{
		rnd = rand() % 2;
		if (rnd == 0) {
			letter = NO_CONNECTION;
		}
		else {
			letter = CONNECTION;
		}
		target[i] = letter;
	}
}



/* init_organism: initialize an organism */
void init_organism(Organism& organ, HN_gene& target) {
	// 0-49 = '?' , 50-74='0' , 75-99='1'
	// assume GENE Already has 20
	int rnd = 0;
	char letter;
	for (int i = 0; i < GENE_SIZE; i++)
	{
		rnd = rand() % 100;
		if (rnd < 50) {
			letter = (char) UNKNOWN;
		}
		else if (rnd < 75) {
			// chose a wrong one - target cannot have a '?' in it
			if (target[i] == CONNECTION) {
				letter = NO_CONNECTION;
			}
			else {
				letter = CONNECTION;
			}
		}
		else {
			letter = target[i]; //correct one
		}
		organ.gene[i] = letter;
	}
}

/* init_pop: initialize the population */
void init_pop(HN_population& pop, HN_gene& target) {
	for (int i = 0; i < POP_SIZE; i++)
	{
		init_organism(pop[i], target);
	}
}


/* hn_organism_solve: organism tries to get to the target */
array<double,3> hn_organism_solve(Organism & organ, HN_gene& target) {
	array<double,3> stats; // [0] = correct , [1] = incorrect , [2] = learned bits
	double correct_num = 0;
	double incorrect_num = 0;
	double learned_num = 0;
	for (int i = 0; i < GENE_SIZE; i++)
	{
		if (organ.gene[i] == target[i]) {
			correct_num++;
		}
		else if (organ.gene[i] != UNKNOWN) { //not correct & not '?'
			incorrect_num++;
		}
	}
	stats[0] = (correct_num / GENE_SIZE);
	stats[1] = (incorrect_num / GENE_SIZE);	
	// now solve and count how many bit are known !
	int trials = 0;
	HN_gene gene = organ.gene;
	bool solved = false;
	for (int trials = 0; trials < TRIALS_LIMIT; trials++)
	{
		gene = organ.gene; // copy the fixed bits
		for (int j = 0; j < GENE_SIZE; j++)
		{
			if (gene[j] == UNKNOWN) {
				gene[j] = (rand() % 2 == 0) ? NO_CONNECTION : CONNECTION; // randomly selects
			}
		}
		solved = true;
		for (int j = 0; j < GENE_SIZE; j++)
		{
			if (gene[j] != target[j]) {
				solved = false;
				break;
			}
		}
		if (solved) {
			// copy the learned bits
			for (int i = 0; i < GENE_SIZE; i++)
			{
				if (organ.gene[i] == UNKNOWN) {
					organ.gene[i] = target[i];
				}
			}
			learned_num = GENE_SIZE - correct_num - incorrect_num; // incorrect_num must be 0 here
			break;//done with the solution
		}
	}
	stats[2] = (learned_num / GENE_SIZE);
	//calculate the fitness
	int N = TRIALS_LIMIT - trials;
	organ.fitness = 1 + (((GENE_SIZE - 1)*N) / TRIALS_LIMIT); // 1+ 19*N/1000

	return stats;

}

/* hn_solve: gives every organsm a TRAIL_NUM chances to get to the target and updates it's fitness */
void hn_solve(HN_population& pop, HN_gene& target, int current_generation) {
	double sum_correct = 0;
	double sum_incorrect = 0;
	double sum_learned = 0;
	array<double,3> tmp; // the results
	for (int i = 0; i < POP_SIZE; i++)
	{
		tmp = hn_organism_solve(pop[i], target);
		sum_correct += tmp[0];
		sum_incorrect += tmp[1];
		sum_learned += tmp[2];
	}
	// calculate the averages
	avg_correct_pos[current_generation] = sum_correct / POP_SIZE;
	avg_incorrect_pos[current_generation] = sum_incorrect / POP_SIZE;
	avg_learned_bits[current_generation] = sum_learned / POP_SIZE;
}


/* hn_get_correct_fitness: returns the chosen fitness from random */
int hn_get_correct_fitness(int rndFit, int maxFit, int minFit, int fitSum) {
	// maxFit -> ... -> minFit
	int fit = maxFit;
	int sum = 0;
	while (sum < fitSum) {
		sum += fit;
		if (rndFit < sum) {
			return fit;
		}
		fit--;
	}
	return minFit;
}

/* hn_get_parent: returns an index of a parent to mutate*/
int hn_get_parent(vector<FitnessPoint>& counters, int maxFit, int minFit, int fitSum) {
	int rndFit = rand() % fitSum;
	int fit = hn_get_correct_fitness(rndFit, maxFit, minFit, fitSum);
	if (counters[fit].length == 0) {
		for (int i = fit + 1; i <= maxFit; i++)
		{
			if (counters[i].length > 0) {
				fit = i;
				break;
			}
		}
	}
	//have a fitness - choose one randomly
	int len = counters[fit].length;
	int relative = rand() % len;
	int pos =  counters[fit].startPos + relative;
	return pos;

}

/* hn_mutate: mutates the population for next generation */
void hn_mutate(HN_population& pop) {
	int max_fitness = pop[0].fitness;
	int min_fitness = pop[POP_SIZE - 1].fitness;
	vector<FitnessPoint> counters(max_fitness + 1); //from 1 to max_fitness (0 is extra position)
	for (int i = 0; i < max_fitness + 1; i++)
	{
		counters[i].length = 0;// start it at 0
	}
	counters[max_fitness].startPos = 0;
	counters[max_fitness].length++;
	int tmp_fit = max_fitness;
	for (int i = 1; i < POP_SIZE; i++)
	{
		if (pop[i].fitness != tmp_fit) {
			counters[tmp_fit].endPos = i - 1;
			tmp_fit = pop[i].fitness;
			counters[tmp_fit].startPos = i;
			counters[tmp_fit].length++;
		}
		else {
			counters[tmp_fit].length++;
		}
	}
	counters[tmp_fit].endPos = POP_SIZE - 1;
	int fitness_sum = 0;
	for (int i = min_fitness; i <= max_fitness; i++)
	{
		fitness_sum += i;
	}
	//now mutate
	HN_population newPop(POP_SIZE);
	int rndFit = 0;
	int gsize = GENE_SIZE / 2;
	for (int i = 0; i < POP_SIZE; i++)
	{

		//choose parents
		int p1 = hn_get_parent(counters, max_fitness, min_fitness, fitness_sum);
		int p2 = hn_get_parent(counters, max_fitness, min_fitness, fitness_sum);
		//mix them
		for (int j = 0; j < gsize; j++)
		{
			newPop[i].gene[j] = pop[p1].gene[j];
		}
		for (int j = gsize; j < GENE_SIZE; j++)
		{
			newPop[i].gene[j] = pop[p2].gene[j];
		}
	}
	//now copy them!
	for (int i = 0; i < POP_SIZE; i++)
	{
		pop[i] = newPop[i];
	}	
}



/* sort by fitness from high to low */
bool hn_fitness_sort(Organism& x, Organism& y)
{
	return (x.fitness > y.fitness);
}

inline void hn_sort_by_fitness(HN_population &population)
{
	sort(population.begin(), population.end(), hn_fitness_sort);
}

/* hn_print_stats_vector: prints everything in a statstics vector as a line seperated with white space*/
void hn_print_stats_vector(vector<double>& vec) {
	for (int i = 0; i < MAX_GENERATIONS; i++)
	{
		cout << vec[i] << " ";
	}
	cout << endl;
}


/* hn_print_statics: prints the statics for all generations */
void hn_print_statics() {
	cout << "There are " << MAX_GENERATIONS << " generations. Here are some statics:" << endl;
	cout << " Average Correct Positions %:" << endl;
	hn_print_stats_vector(avg_correct_pos);
	cout << " Average Incorrect Positions %:" << endl;
	hn_print_stats_vector(avg_incorrect_pos);
	cout << " Average Learned Bits %:" << endl;
	hn_print_stats_vector(avg_learned_bits);
}


//============== Main HW3 ==============
int main() {
	srand(unsigned(time(NULL)));
	HN_population population(POP_SIZE);
	HN_gene target; //the target configuration
	init_target(target); // intialize target
	init_pop(population, target);

	// for on all of them
	// inside print statistics
	for (int i = 0; i < MAX_GENERATIONS; i++)
	{
		hn_solve(population, target, i);
		hn_sort_by_fitness(population); //sort them by fitness
		hn_mutate(population); // new population
	}
	hn_print_statics();
	return 0;
}

