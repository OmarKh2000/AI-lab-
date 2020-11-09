// Genetic5.cpp : Defines the entry point for the console application.
//


#pragma warning(disable:4786)		// disable debug warning

#include <iostream>					// for cout etc.
#include <vector>					// for vector class
#include <string>					// for string class
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()

#define GA_POPSIZE		2048		// ga population size
#define GA_MAXITER		16384		// maximum iterations
//#define GA_MAXITER		1638		// maximum iterations
#define GA_ELITRATE		0.10f		// elitism rate
#define GA_MUTATIONRATE	0.25f		// mutation rate
#define GA_MUTATION		RAND_MAX * GA_MUTATIONRATE
#define GA_TARGET		std::string("Hello world!")

#define TARGET_HIT_HEURISTIC_BONUS 0 // bonus to give for each correct letter guessing
#define TARGET_HIT_HEURISTIC_BAD_GUESS_POINTS 1 //points to give for a bad guess
#define START_AGE 0 //the starting age of a gene
#define MAX_AGE 5 //maximum generations to stay
#define ADULT_AGE 3 //adult age - the most effective and powerful age o survive
#define MAX_AGEING_FITNESS 100 //to kill old genes we give them MAX_AGING_FITNESS
#define SELECTION_K 2 //default K to use in tournaments

using namespace std;				// polluting global namespace, but hey...

struct ga_struct
{
	string str;						// the string
	unsigned int fitness;			// its fitness
	int age;						// it's age - start at 0
	unsigned int aging_fitness;			// its fitness considernig the age
};

typedef vector<ga_struct> ga_vector;// for brevity

void init_population(ga_vector &population,
					 ga_vector &buffer )
{
	int tsize = GA_TARGET.size();

	for (int i=0; i<GA_POPSIZE; i++) {
		ga_struct citizen;

		citizen.fitness = 0;
		citizen.str.erase();

		//=== added these ===
		citizen.age = START_AGE;
		citizen.aging_fitness = 0;
		//=== till here ===

		for (int j=0; j<tsize; j++)
			citizen.str += (rand() % 90) + 32;

		population.push_back(citizen);
	}

	buffer.resize(GA_POPSIZE);
}

void calc_fitness(ga_vector &population)
{
	string target = GA_TARGET;
	int tsize = target.size();
	unsigned int fitness;

	for (int i=0; i<GA_POPSIZE; i++) {
		fitness = 0;
		for (int j=0; j<tsize; j++) {
			fitness += abs(int(population[i].str[j] - target[j]));
		}

		population[i].fitness = fitness;
	}
}

bool fitness_sort(ga_struct x, ga_struct y)
{ return (x.fitness < y.fitness); }

inline void sort_by_fitness(ga_vector &population)
{ sort(population.begin(), population.end(), fitness_sort); }

void elitism(ga_vector &population,
				ga_vector &buffer, int esize )
{
	for (int i=0; i<esize; i++) {
		buffer[i].str = population[i].str;
		buffer[i].fitness = population[i].fitness;
	}
}

void mutate(ga_struct &member)
{
	int tsize = GA_TARGET.size();
	int ipos = rand() % tsize;
	int delta = (rand() % 90) + 32;

	member.str[ipos] = ((member.str[ipos] + delta) % 122);
}

void mate(ga_vector &population, ga_vector &buffer)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int tsize = GA_TARGET.size(), spos, i1, i2;

	elitism(population, buffer, esize);

	// Mate the rest
	for (int i=esize; i<GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		spos = rand() % tsize;

		buffer[i].str = population[i1].str.substr(0, spos) +
			            population[i2].str.substr(spos, tsize - spos);

		if (rand() < GA_MUTATION) mutate(buffer[i]);
	}
}

inline void print_best(ga_vector &gav)
{ cout << "Best: " << gav[0].str << " (" << gav[0].fitness << ")" << endl; }

inline void swap(ga_vector *&population,
				 ga_vector *&buffer)
{ ga_vector *temp = population; population = buffer; buffer = temp; }


//============== Time & Deviation ==============

/* calculate_fitness_avg: calculates and returns the fitness average of the current population
*	 @param square: if true , the fitness will be added as (fitness)^2 instead of fitness - useful for variance calculation
*/
double calculate_fitness_avg(vector<ga_struct>& population, bool square = false) {
	unsigned int sum = 0;
	unsigned int fitness = 0;
	for (int i = 0; i < GA_POPSIZE; i++) {
		fitness = population[i].fitness;
		if (square) {
			fitness *= fitness; // fitness = fitness*fitness;
		}
		sum += fitness;
	}
	double avg = sum / GA_POPSIZE;
	return avg;
}

/* calculate_fitness_deviation: calculates and returns the standard deviationof the population's fitness */
double calculate_fitness_deviation(vector<ga_struct>& population) {
	/* the nest 2 lines could be don with one call to calculate_fitness_avg */
	double x_square = calculate_fitness_avg(population, true); // E[x^2]
	double mean = calculate_fitness_avg(population, false);// E[x]
	mean *= mean;// (E[x])^2
	double variance = x_square - mean; // E[x^2] - (E[x])^2
	//get sqrt of variance and return it
	double deviation = sqrt(variance);
	return deviation;
}

/* printf_fitness_avg: calculates and prints the fitness average of population */
void printf_fitness_avg(vector<ga_struct>& population) {
	cout << "Fitness average: " << calculate_fitness_avg(population) << " -- Deviation: "<< calculate_fitness_deviation(population) << endl;
}



/* calculate_and_print_elapsed_time: calculates and prints the elapsed time from a given start time */
clock_t calculate_and_print_elapsed_time(clock_t startTime) {
	clock_t stopTime = clock();
	clock_t elapsedTime = stopTime - startTime;
	cout << "Generation time: " << elapsedTime << " clock ticks = " << (((float)elapsedTime) / CLOCKS_PER_SEC )<< " seconds" << endl;
	return stopTime;
}


//============== Target hit heuristic ==============

/* calculate_target_hit_heuristic: calculates and returns the heuristic value of a certin guess in target hit herurostic */
int calculate_target_hit_heuristic(string& guess) {
	int res = 0;
	string target = GA_TARGET;
	int i=0;
	for (i = 0; i < guess.length() ; i++)
	{
		if (guess[i] == target[i]) {
			res += TARGET_HIT_HEURISTIC_BONUS;
		}
		else {
			res += TARGET_HIT_HEURISTIC_BAD_GUESS_POINTS;
		}
	}
	return res;
}

/* calc_fitness_using_heuristic: calculates the fitness function and addes heuristic using the target hit heuristic for each gene */
void calc_fitness_using_heuristic(ga_vector &population)
{
	string target = GA_TARGET;
	int tsize = target.size();
	calc_fitness(population);
	for (int i = 0; i < GA_POPSIZE; i++) {
		population[i].fitness += calculate_target_hit_heuristic(population[i].str);
	}
}

//============== SUS ==============

/* get_sum_of_fitenesses: returns the sum of the fitnesses of the population  */
unsigned int get_sum_of_fitenesses(ga_vector &population) {

	unsigned int sum = 0;
	for (int i = 0; i < GA_POPSIZE; i++)
	{
		sum += population[i].fitness;
	}
	return sum;
}


/* get_next_elite_sus: calculates and returns the index of the next elite int population
*  update tmp fitness sum also for time saving
*/
int get_next_elite_sus(ga_vector &population, unsigned int real_fitness_sum,unsigned int* tmp_fitness_sum,int target, int start_at) {
	if (start_at >= GA_POPSIZE) {
		start_at = 0;//check if can change it
	}
	int count = 0;
	for (int j = start_at; j < GA_POPSIZE; j++)
	{
		if (*tmp_fitness_sum + population[j].fitness > target) {
			return j;
		}
		//else
		*tmp_fitness_sum += population[j].fitness;
		//check for overflow and next circle
		if (*tmp_fitness_sum > real_fitness_sum) {
			*tmp_fitness_sum = *tmp_fitness_sum % real_fitness_sum;
			target = target % real_fitness_sum;//here and not in the begininng because
												//if we do it in the beginning target will be less than tmp sum and will return a wrong result
			//reset the index to start over
			j = 0;
			count++;
		}
		if (count > 1) {
			break;//to eliminate infinate loops- if count> 1 something is wrong !
		}
	}
	return 0;//mustn't come here!
}

/* elitism_sus: choose who servives for the next round */
void elitism_sus(ga_vector &population, ga_vector &buffer, int esize) { //works oppositly
	unsigned int fitness_sum = get_sum_of_fitenesses(population); // the fittest has the least fitness
	int first = rand() % (fitness_sum);
	double prop = first / fitness_sum; // a proportion of sum
	unsigned int tmp_fitness_sum = 0;
	int target = first;
	int tmp_index = 0;//start at 0
	int step = (1 / esize) * fitness_sum;// how much to add each time - int for easier access
	if (step == 0) { // to make sure that we move on
		step = 1;
	}
	//get the elitism
	for (int i = 0; i < esize; i++) {
		tmp_index = get_next_elite_sus(population, fitness_sum, &tmp_fitness_sum, target, tmp_index);//start at the beginning
		buffer[i].str = population[tmp_index].str;
		buffer[i].fitness = population[tmp_index].fitness;
		//update target
		target += step;
	}
}

/* mate_rest_sus: mates the rest of the population after the elitism were chosen in sus */
void mate_rest_sus(ga_vector &population, ga_vector &buffer, int esize) {
	// Mate the rest
	int tsize = GA_TARGET.size(), spos, i1, i2;
	for (int i = esize; i < GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		spos = rand() % tsize;

		buffer[i].str = population[i1].str.substr(0, spos) +
			population[i2].str.substr(spos, tsize - spos);

		if (rand() < GA_MUTATION) mutate(buffer[i]);
	}
}


/* mate_sus: chooses the elitism using sus and then mutates the rest */
void mate_sus(ga_vector &population, ga_vector &buffer)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int tsize = GA_TARGET.size(), spos, i1, i2;

	//choose the elitisim
	elitism_sus(population, buffer, esize);

	// Mate the rest
	mate_rest_sus(population, buffer, esize);
}

//============== Aging ==============

/* calculate_aging_fitness: calculates the aging fitness for each gene */
void calculate_aging_fitness(ga_vector &population) {
	unsigned int tmp_fitness = 0;
	int age_factor = 0;
	for (int i = 0; i < GA_POPSIZE; i++)
	{
		tmp_fitness = population[i].fitness;
		age_factor = abs(population[i].age - ADULT_AGE);
		if (age_factor < 1) {// for case age = 0
			age_factor = 1;
		}
		population[i].aging_fitness = age_factor * tmp_fitness;
	}
}


/* aging_fitness_compare: returns true if x.aging_fitness is less than y.aging_fitness , false otherwise */
bool aging_fitness_compare(ga_struct x, ga_struct y)
{
	return (x.aging_fitness < y.aging_fitness);
}


/* sort_aging_fitness: reorders the population by fitness and aging */
inline void sort_by_aging_fitness(ga_vector &population)
{
	sort(population.begin(), population.end(), aging_fitness_compare);
}

/* increment_age: increments age by one for each gene in hte population - recieves psize to allow buffer instead of population */
void increment_age(ga_vector &population, int psize) {
	for (int i = 0; i < psize; i++)
	{
		population[i].age++;
		if (population[i].age > MAX_AGE) {
			//must die
			population[i].aging_fitness = MAX_AGEING_FITNESS;//wont be selected next time
		}
	}
}

/* elitism_aging: choose who servives for the next round considering the age */
void elitism_aging(ga_vector &population, ga_vector &buffer, int esize) {

	// calculate the aging fitness
	calculate_aging_fitness(population);
	//increment age and kill old genes
	increment_age(population, GA_POPSIZE);
	//get the elitism
	sort_by_aging_fitness(population);
	elitism(population, buffer, esize);//this copies the first esize genes
}

/* mate_rest_aging: mates the rest of the population after the elitism were chosen by aging fitness */
void mate_rest_aging(ga_vector &population, ga_vector &buffer, int esize) {
	// Mate the rest
	int tsize = GA_TARGET.size(), spos, i1, i2;
	for (int i = esize; i < GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		spos = rand() % tsize;

		buffer[i].age = START_AGE; //reset the age for a new born

		buffer[i].str = population[i1].str.substr(0, spos) +
			population[i2].str.substr(spos, tsize - spos);

		if (rand() < GA_MUTATION) mutate(buffer[i]);
	}
}

/* mate_aging: chooses the elitism using aging fitness and then mutates the rest */
void mate_aging(ga_vector &population, ga_vector &buffer)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int tsize = GA_TARGET.size(), spos, i1, i2;

	//choose the elitisim
	elitism_aging(population, buffer, esize);
	// Mate the rest
	mate_rest_aging(population, buffer, esize);
}

/* print_best_aging: prints the best gene with it's age and fitness*/
inline void print_best_aging(ga_vector &gav)
{
	cout << "Best: " << gav[0].str << " (" << gav[0].fitness << ") age = " << gav[0].age <<endl;
}

//==============  Selection Tournament ==============

/* select_genes: selects K genes at random from the population and fills them in the buffer, then sorts them by fitness */
void select_genes(ga_vector &population, ga_vector &tmp_buffer, int K) {
	bool flags[GA_POPSIZE]; //if flage[i]=true then we already have gene i in tmp_buffer
	//reset flags
	for (int i = 0; i < GA_POPSIZE; i++){
		flags[i] = false;
	}
	//get K genes
	int index = 0;
	int counter = 0;//to stop infine loops
	for (int i = 0; i < K; i++)
	{
		index = rand() % (GA_POPSIZE);
		if (flags[index]) {
			counter++;
			if (counter < 3){//re-loop
				i--;
				continue;
			}
		}
		//ok now copy
		tmp_buffer[i].str = population[index].str;
		tmp_buffer[i].fitness = population[index].fitness;
		counter = 0; //reset the counter
		flags[index] = true;//update flag
	}
	sort_by_fitness(tmp_buffer);//it knows the length on it's own
}

/* elitism_selection_tournament: select at random K members then the best , do it for all elitisim */
void elitism_selection_tournament (ga_vector &population, ga_vector &buffer, int esize,int K) {
	//pick K at random
	ga_vector tmp_buffer;
	tmp_buffer.resize(K);
	for (int i = 0; i < esize; i++) {
		select_genes(population, tmp_buffer, K);
		//get the best one
		buffer[i].str = tmp_buffer[0].str;
		buffer[i].fitness = tmp_buffer[0].fitness;
		//age and aging_fitness not important here

		//check the if they have to be different!
	}

}

/* mate_rest_selection: mates the rest of the population after the elitism were chosen by aging fitness */
void mate_rest_selection(ga_vector &population, ga_vector &buffer, int esize) {
	// Mate the rest
	int tsize = GA_TARGET.size(), spos, i1, i2;
	for (int i = esize; i < GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		spos = rand() % tsize;

		buffer[i].age = START_AGE; //reset the age for a new born

		buffer[i].str = population[i1].str.substr(0, spos) +
			population[i2].str.substr(spos, tsize - spos);

		if (rand() < GA_MUTATION) mutate(buffer[i]);
	}
}

/* mate_selection: chooses the elitism using tournamet selection  and then mutates the rest */
void mate_selection(ga_vector &population, ga_vector &buffer , int K = SELECTION_K)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int tsize = GA_TARGET.size(), spos, i1, i2;

	//choose the elitisim
	elitism_selection_tournament(population, buffer, esize,K);
	// Mate the rest
	mate_rest_selection(population, buffer, esize);
}

//============== Cross Points ==============

/* mate_two_point_crossover: mates using 2 crossover points */
void mate_two_point_crossover(ga_vector &population, ga_vector &buffer)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int tsize = GA_TARGET.size(), spos1,spos2, i1, i2;
	int tmppos;
	elitism(population, buffer, esize);

	// Mate the rest
	for (int i = esize; i < GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		spos1 = rand() % tsize;
		spos2 = rand() % tsize;
		if (spos1 > spos2) {//switch - we want spos1<spos2
			tmppos = spos1;
			spos1 = spos2;
			spos2 = tmppos;
		}
		if (spos1 == spos2) {//solve the same point problem
			if (spos1 > 1) {
				spos1--;
			}
			else {
				spos1 = 1;
				spos2 += 2;
			}
		}

		buffer[i].str = population[i1].str.substr(0, spos1) +
			population[i2].str.substr(spos1, spos2 - spos1) +
			population[i2].str.substr(spos2, tsize - spos2);

		if (rand() < GA_MUTATION) mutate(buffer[i]);
	}
}

/* mate_uniform_crossover: mates using uniform crossover  */
void mate_uniform_crossover(ga_vector &population, ga_vector &buffer)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int tsize = GA_TARGET.size(), spos, i1, i2;
	int rnd;
	elitism(population, buffer, esize);

	int length = GA_TARGET.length();//target length
	string st = GA_TARGET;//set the size/length
	// Mate the rest
	for (int i = esize; i < GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		//get a permutation + build string
		for (int j = 0; j < length; j++)
		{
			rnd = rand() % 2; //get 0 or 1
			st[j] = (rnd == 0) ? population[i1].str[j] : population[i2].str[j];
		}

		buffer[i].str = st;

		if (rand() < GA_MUTATION) mutate(buffer[i]);
	}
}

//============== Main ==============
int main()
{

	bool solved = false;

	srand(unsigned(time(NULL)));

	ga_vector pop_alpha, pop_beta;
	ga_vector *population, *buffer;

	init_population(pop_alpha, pop_beta);
	population = &pop_alpha;
	buffer = &pop_beta;

	clock_t startTimeStamp = clock();
	clock_t prevTimeStamp = startTimeStamp;
	for (int i=0; i<GA_MAXITER; i++) {
		calc_fitness(*population);		// calculate fitness
		//calc_fitness_using_heuristic(*population); // calculate using the hit target heuristic

		sort_by_fitness(*population);	// sort them
		print_best(*population);		// print the best one
		//print_best_aging(*population);

		printf_fitness_avg(*population); //print the average
		prevTimeStamp = calculate_and_print_elapsed_time(prevTimeStamp);// print the elapsed time

		if ((*population)[0].fitness == 0) { // it will reach 0 fitness as always because it has no idea what is it
			clock_t elapsedTime = clock();
			elapsedTime = elapsedTime - startTimeStamp;
			cout << "Reached a local optima with time: " << elapsedTime << " clock ticks  = " << (((float)elapsedTime) / CLOCKS_PER_SEC) << " seconds" << endl;
			solved = true;
			break;
		}


		//mate(*population, *buffer);		// mate the population together
		mate_sus(*population, *buffer);		// mate the population together
		//mate_aging(*population, *buffer);//mate using aging
		//mate_selection(*population, *buffer);//mate using aging
		//mate_two_point_crossover(*population, *buffer);
		//mate_uniform_crossover(*population, *buffer);

		//swap
		swap(population, buffer);		// swap buffers
	}

	if (!solved) {
		clock_t elapsedTime = clock();
		elapsedTime = elapsedTime - startTimeStamp;
		cout<<"Failed to find the solution after time: " << elapsedTime << " clock ticks  = " << (((float)elapsedTime) / CLOCKS_PER_SEC) << " seconds" << endl;
	}
	return 0;
}
