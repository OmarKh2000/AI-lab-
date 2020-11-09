// knapSack.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>					// for cout etc.
#include <vector>					// for vector class
#include <string>					// for string class
#include <algorithm>				// for sort algorithm
#include <algorithm>    // std::copy

#include <time.h>					// for random seed
#include <math.h>					// for abs()
#define HAVE_STRUCT_TIMESPEC

#include <pthread.h>

//#define GA_POPSIZE		2048		// ga population size
#define GA_POPSIZE		512		// ga population size
//#define GA_MAXITER		16384		// maximum iterations
#define GA_MAXITER		1638		// maximum iterations
#define GA_ELITRATE		0.10f		// elitism rate
#define GA_MUTATIONRATE	0.25f		// mutation rate
#define GA_MUTATION		RAND_MAX * GA_MUTATIONRATE
#define NUM_THREADS 8

const int WEIGHTS[] = { 12,7,11,8,9 };
const int PROFITS[] = { 24,13,23,15,16 };
#define LENGTH 5
#define CAPACITY 26
using namespace std;				// polluting global namespace, but hey...

typedef struct qns_gene_struct {
	vector<int> items; //like str
	int weight;
	int value;
	int fitness;
}Q_Gene;
typedef vector<Q_Gene> qng_vector; //this is the population

struct args {
	int thread_id;
	qng_vector population;
};


void init_queen_population(qng_vector &population, qng_vector &buffer)
{

	for (int i = 0; i < GA_POPSIZE; i++) {
		Q_Gene citizen;
		citizen.fitness = 0;
		citizen.weight = 0;
		citizen.value = 0;
		citizen.items.resize(LENGTH, 0);
		for (int i = 0; i < LENGTH; i++) {
			citizen.items[i] = (rand() % 2);
		}
		population.push_back(citizen);
	}

	buffer.resize(GA_POPSIZE);
}

bool fitness_sort_queens(Q_Gene x, Q_Gene y)
{
	return (x.fitness < y.fitness);
}

inline void sort_by_fitness_queens(qng_vector &population)
{
	sort(population.begin(), population.end(), fitness_sort_queens);
}

void calc_fitness(qng_vector &population)
{
	int fitness_count;
	int weight;

	for (int i = 0; i < GA_POPSIZE; i++) {
		fitness_count = 0;
		weight = 0;

		for (int j = 0; j < LENGTH; j++) {
			weight += (population[i].items[j] == 1) ? WEIGHTS[j] : 0;
			fitness_count += (population[i].items[j] == 1) ? -PROFITS[j] : 0;
		}
		if (weight > CAPACITY)
		{
			population[i].fitness = INT_MAX;
			continue;
		}
		population[i].fitness = fitness_count;
	}
}


/* elitism_queens: get who will survive for the next round */
void elitism_queens(qng_vector &population,
	qng_vector &buffer, int esize)
{
	for (int i = 0; i < esize; i++) {
		buffer[i].items = population[i].items;
		buffer[i].fitness = population[i].fitness;
		buffer[i].weight = population[i].weight;
		buffer[i].value = population[i].value;
	}
}

void mate(qng_vector &population, qng_vector &buffer)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int lim, i1, i2;

	elitism_queens(population, buffer, esize);

	// Mate the rest
	for (int i = esize; i < GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		lim = rand() % LENGTH;//how many queens to copy form the first
		for (int j = 0; j < lim; j++)
		{
			buffer[i].items.push_back(population[i1].items[j]);
		}
		for (int k = lim; k < LENGTH; k++)
		{
			buffer[i].items.push_back(population[i2].items[k]);
		}
	}
}

inline void swap_queens(qng_vector *&population,
	qng_vector *&buffer)
{
	qng_vector *temp = population; population = buffer; buffer = temp;
}

void *calc_fitness_thread(void *input) {

	struct args *my_data;
	my_data = (struct args *) input;

	int fitness_count;
	int weight;
	int porbationSize = GA_POPSIZE / NUM_THREADS;
	for (int i = my_data->thread_id*porbationSize; i <( my_data->thread_id+1)*porbationSize; i++) {
		fitness_count = 0;
		weight = 0;

		for (int j = 0; j < LENGTH; j++) {
			weight += (my_data->population[i].items[j] == 1) ? WEIGHTS[j] : 0;
			fitness_count += (my_data->population[i].items[j] == 1) ? -PROFITS[j] : 0;
		}
		if (weight > CAPACITY)
		{
			my_data->population[i].fitness = INT_MAX;
			continue;
		}
		my_data->population[i].fitness = fitness_count;
	}
	return NULL;
	//pthread_exit(NULL);
}

int main()
{
	bool solved = false;

	srand(unsigned(time(NULL)));

	qng_vector pop_alpha, pop_beta;
	qng_vector *population, *buffer;

	init_queen_population(pop_alpha, pop_beta);
	population = &pop_alpha;
	buffer = &pop_beta;



	clock_t startTimeStamp = clock();
	clock_t prevTimeStamp = startTimeStamp;
	for (int i = 0; i < GA_MAXITER; i++) {
		pthread_t threads[NUM_THREADS];
		args args[NUM_THREADS];
		//calc_fitness(*population);		// calculate fitness
		//calc_fitness_using_heuristic(*population); // calculate using the hit target heuristic
		int rc=0;

		for (int c = 0; c < NUM_THREADS; c++) {	// create threads
			//cout << "main() : creating thread, " << i << endl;
			args[c].thread_id = c;
			//std::copy(&population, population->end(), args[i].population.begin());
			args[c].population.resize (population->size());
			for (int k = 0; k < population->size(); k++) {
				args[c].population[k] = population->at(k);
			}
			//args[i].population = population;
			//rc = pthread_create(&threads[i], NULL, calc_fitness_thread, (void*)&args[i]);

			
		}
	//	pthread_exit(NULL);

		void *status;

		for (int c = 0; c < NUM_THREADS; c++) {	// wait for all threads
		//	rc = pthread_join(threads[c], &status);
			
			//cout << "Main: completed thread id :" << i;
//			cout << "  exiting with status :" << status << endl;
		}

		calc_fitness(*population);


		sort_by_fitness_queens(*population);	// sort them
		//print_best_aging(*population);


		if ((*population)[0].fitness == 0) { // it will reach 0 fitness as always because it has no idea what is it
			clock_t elapsedTime = clock();
			elapsedTime = elapsedTime - startTimeStamp;
			cout << "Reached a local optima with time: " << elapsedTime << " clock ticks  = " << (((float)elapsedTime) / CLOCKS_PER_SEC) << " seconds" << endl;
			cout << "Items are:" << endl;
			for (int i = 0; i < LENGTH; i++) {
				cout << (*population)[0].items[i] << endl;
			}
			cout << "value is:" << 0 - ((*population)[0].fitness);
			solved = true;
			break;
		}


		//mate(*population, *buffer);		// mate the population together
		//mate_sus(*population, *buffer);		// mate the population together
		//mate_aging(*population, *buffer);//mate using aging
		//mate_selection(*population, *buffer);//mate using aging
		//mate_two_point_crossover(*population, *buffer);
		//mate_uniform_crossover(*population, *buffer);

		mate(*population, *buffer);

		//swap
		swap_queens(population, buffer);		// swap buffers
	}

	if (!solved) {
		clock_t elapsedTime = clock();
		elapsedTime = elapsedTime - startTimeStamp;

		cout << elapsedTime << " clock ticks  = " << (((float)elapsedTime) / CLOCKS_PER_SEC) << " seconds" << endl;
		cout << "Items are:" << endl;
		for (int i = 0; i < LENGTH; i++) {
			cout << (*population)[0].items[i] << endl;
		}
		cout << "value is:" << 0 - ((*population)[0].fitness);
	}
	return 0;
}

