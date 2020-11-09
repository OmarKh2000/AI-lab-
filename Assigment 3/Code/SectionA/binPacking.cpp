// Genetic5.cpp : Defines the entry point for the console application.
//


#pragma warning(disable:4786)		// disable debug warning
#include <fstream>
#include <iostream>					// for cout etc.
#include <vector>					// for vector class
#include <string>					// for string class
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()

#define GA_POPSIZE		2048		// ga population size
#define GA_MAXITER		10000		// maximum iterations
#define GA_ELITRATE		0.10f		// elitism rate
#define GA_MUTATIONRATE	0.25f		// mutation rate
#define GA_MUTATION		RAND_MAX * GA_MUTATIONRATE
#define GA_TARGET		100
#define K 3
#define SHARING_FITTNESS_RADIUS 8
#define SIMILARITY_METRIC 15
#define LOCAL_OPTIMA_SIMILARITY  10
#define LOCAL_OPTIMA_AVG 10
#define LOCAL_OPTIMA_MUTATE 256
#define LOCAL_OPTIMA_GENERATE_NEW 256
using namespace std;				// polluting global namespace, but hey...


static int SIZE;
static int C;
vector<int> Weights;
int localOptimaSimlairty = 0;
int localOptimaAvg = 0;
int Mutation_Factor = 0;

struct ga_struct
{
	vector<int> items;

	 int fitness;			// its fitness
};

typedef vector<ga_struct> ga_vector;// for brevity

void init_population(ga_vector &population,
	ga_vector &buffer)
{	
	int tsize = SIZE;
	//population.resize(SIZE);
	for (int i = 0; i < GA_POPSIZE; i++) {
		ga_struct citizen;

		citizen.fitness = 0;
		citizen.items.resize(SIZE);

		for (int j = 0; j < tsize; j++)
			citizen.items[j] = (rand() % SIZE);

		population.push_back(citizen);
	}

	buffer.resize(GA_POPSIZE);
}

bool getFileContent(std::string fileName)
{

	// Open the File
	std::ifstream in(fileName.c_str());

	// Check if object is valid
	if (!in)
	{
		std::cerr << "Cannot open the File : " << fileName << std::endl;
		return false;
	}

	std::string str;
	// Read the next line from File untill it reaches the end.
	int i = 0;
	while (std::getline(in, str))
	{
		

		// Line contains string of length > 0 then save it in vector
		if (str.size() > 0) {
			if (i == 0) {
				SIZE = stoi(str);
				i++;
				continue;
			}
			if (i == 1) {
				 C= stoi(str);
				 i++;
				 continue;
			}
			Weights.push_back(stoi(str));
		}
	}
	//Close The File
	in.close();
	return true;
}


void calc_fitness(ga_vector &population)
{
	//string target = GA_TARGET;
	int tsize = SIZE;
	unsigned int fitness;


	vector<int> bins(SIZE,0);

	int binsNum = 0;
	for (int i = 0; i < GA_POPSIZE; i++) {
		fitness = 0;

		for (int j = 0; j < SIZE; j++) {
			int k = population[i].items[j];
			binsNum += (bins[population[i].items[j]] == 0 ? 1 : 0);
			bins[population[i].items[j]] += Weights[j];
		}

		for (int j = 0; j < SIZE; j++) {
			
			fitness += pow(bins[j] / C, K);
		}

		bins.clear();
		bins.resize(SIZE, 0);
		population[i].fitness = fitness / binsNum;
		binsNum = 0;

	}
}

bool fitness_sort(ga_struct x, ga_struct y)
{
	return (x.fitness > y.fitness);
}

inline void sort_by_fitness(ga_vector &population)
{
	sort(population.begin(), population.end(), fitness_sort);
}

void elitism(ga_vector &population,
	ga_vector &buffer, int esize)
{
	for (int i = 0; i < esize; i++) {
		buffer[i].items = population[i].items;
		buffer[i].fitness = population[i].fitness;
	}
}

/*void mutate(ga_struct &member)
{
	int tsize = SIZE;
	int ipos = rand() % tsize;
	int delta = (rand() % 90) + 32;

	member.str[ipos] = ((member.str[ipos] + delta) % 122);
}*/

void mate(ga_vector &population, ga_vector &buffer)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int tsize = SIZE, spos, i1, i2;

	//elitism(population, buffer, esize);
	elitism(population, buffer, GA_POPSIZE);

	// Mate the rest
	for (int i = esize; i < GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		spos = rand() % tsize;

		for (int j = 0; j < spos; j++) {
			buffer[i].items[j] = population[i1].items[j];
		}
		for (int j = spos; j < tsize; j++) {
			buffer[i].items[j] = population[i2].items[j];

		}
		
	}
}

inline int print_best(ga_vector &gav)
{
	vector<int> bins(SIZE, 0);

	int binsNum = 0;
	for (int j = 0; j < SIZE; j++) {
		int k = gav[0].items[j];
		binsNum += (bins[gav[0].items[j]] == 0 ? 1 : 0);
		bins[gav[0].items[j]] += Weights[j];
	} 

	
	//cout << "Best: " << binsNum << endl;
	return binsNum;
}

void mutate(ga_struct &member)
{
	int tsize = SIZE;
	int ipos = rand() % tsize;
	int delta = (rand() % 90) + 32;

	member.items[ipos] = ((member.items[ipos] + delta) % 122);
}

inline void swap(ga_vector *&population,
	ga_vector *&buffer)
{
	ga_vector *temp = population; population = buffer; buffer = temp;
}

int levenshteinDistance(ga_struct g1, ga_struct g2) {
	int distance = 0;
	for (int i = 0; i < SIZE; i++) {
		if (g1.items[i] != g2.items[i]) {
			distance++;
		}
	}
	return distance;

}

void fittnesShare(ga_vector &population) {
	calc_fitness(population);		// calculate fitness
	vector<int> min(SHARING_FITTNESS_RADIUS, INT_MAX);
	for (int i = 0; i < GA_POPSIZE; i++) {
		min[i / GA_POPSIZE] = std::min(min[i / GA_POPSIZE], population[i].fitness);
	}

	for (int i = 0; i < GA_POPSIZE; i++) {
		population[i].fitness= std::min(min[i / GA_POPSIZE], population[i].fitness);
	}
}

void Crowding(ga_vector &population, ga_vector &buffer) {
	int esize = GA_POPSIZE * GA_ELITRATE;
	int tsize = SIZE, spos, i1, i2;

	//elitism(population, buffer, esize);
	elitism(population, buffer, GA_POPSIZE);

	// Mate the rest
	for (int i = esize; i < GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		spos = rand() % tsize;
		if (SIMILARITY_METRIC <= levenshteinDistance(population[i1], population[i2])) {
			continue;
		}
		for (int j = 0; j < spos; j++) {
			buffer[i].items[j] = population[i1].items[j];
		}
		for (int j = spos; j < tsize; j++) {
			buffer[i].items[j] = population[i2].items[j];

		}
		if (rand() < GA_MUTATION+ Mutation_Factor) mutate(buffer[i]);
	}
	

	
}
void generateNew(ga_vector &population) {
	int randomGen = 0;
	for (int i = 0; i < LOCAL_OPTIMA_GENERATE_NEW; i++) {
		randomGen = rand() % GA_POPSIZE;
		for (int j = 0; j < SIZE; j++)
			population[randomGen].items[j] = (rand() % SIZE);
	}

}
void createOffSpring(ga_vector &population, vector<vector<int>> values) {	// create OFFSPRING
	int i1, i2;
	for (int i = GA_POPSIZE; i < 2 * GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE);
		i2 = rand() % (GA_POPSIZE);
		int spos = rand() % SIZE;



		ga_struct citizen;
		citizen.fitness = 0;
		citizen.items.resize(SIZE);




		for (int j = 0; j < spos; j++) {
			citizen.items[j] = population[i1].items[j];
		}
		for (int j = spos; j < SIZE; j++) {
			citizen.items[j] = population[i2].items[j];

		}
		population[i] = citizen;
	}



}
void sortByCrowding(vector<int> &items, vector<vector<int>>& values) {
	vector<double> crowdingDistance;
	int minIndex = 0, maxIndex = 0;
	int size = items.size();




	for (int i = 0; i < items.size(); i++) {	// find min and max
		minIndex = values[i][0] < values[minIndex][0] ? i : minIndex;
		maxIndex = values[i][0] > values[maxIndex][0] ? i : minIndex;


	}
	int j = 0;
	double delta = pow(pow(values[maxIndex][0] - values[minIndex][0], 2) + pow(values[maxIndex][1] - values[minIndex][1], 2), 0.5);	// calculate crodwing distance
	for (int i = 0; i < size; i++) {
		double delta2 = pow(pow(values[(i + size - 1) % size][0] - values[(i + 1) % size][0], 2) + pow(values[(i + size - 1) % size][1] - values[(i + 1) % size][1], 2), 0.5);
		crowdingDistance.push_back((delta2) / delta);
	}
	for (int i = 0; i < items.size(); i++) {	// sort items by crodwing distance
		for ( j; j < size; j++) {
			if (crowdingDistance[i] > crowdingDistance[j] && i < j) {
				double temp = crowdingDistance[i];
				crowdingDistance[i] = crowdingDistance[j];
				crowdingDistance[j] = temp;
				temp = items[i];
				items[i] = items[j];
				items[j] = temp;

			}

		}
	}


}

void localOptima(ga_vector &population) {
	int i1, i2;
	localOptimaAvg = 0;
	localOptimaSimlairty = 0;
	for (int i = 0; i < GA_POPSIZE; i++) {
		localOptimaAvg += (population[i].fitness/ GA_POPSIZE);
	}

	for (int i = 1; i < GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE);
		i2 = rand() % (GA_POPSIZE);
		localOptimaSimlairty += (levenshteinDistance(population[i1], population[i2]) / GA_POPSIZE);
	}

	if ( (localOptimaAvg > LOCAL_OPTIMA_AVG) || (localOptimaSimlairty > LOCAL_OPTIMA_SIMILARITY)) {
		//for (int i = 0; i < LOCAL_OPTIMA_MUTATE; i++) {	// mutate randomly
			//if (rand() < GA_MUTATION) mutate(population[rand() % GA_POPSIZE]);
		//}
		Mutation_Factor = localOptimaAvg / LOCAL_OPTIMA_AVG + localOptimaSimlairty / LOCAL_OPTIMA_SIMILARITY;
		generateNew(population);	// generate a new random gens
	}


}
void NSGA2(ga_vector &population) {	// NSGA2 algorthim
	ga_vector result; 


	std::vector<vector<int>> values(2*GA_POPSIZE);	// vector of x,y and rank values.
	std::vector<vector<int>> dominates(GA_POPSIZE);
	std::vector<int> dominatedCounter(GA_POPSIZE, 0);
	for (int i = 0; i < GA_POPSIZE; i++) {
		values[i].push_back(-(pow(population[i].fitness,2)));
		values[i].push_back(-(pow(population[i].fitness-2, 2)));
	}

	for (int i = 0; i < GA_POPSIZE; i++) {
		for (int j = 0; j < GA_POPSIZE; j++) {
			if ((values[i][0] <= values[j][0] && values[i][1] <= values[j][1]) && (values[i][0] < values[j][0] || values[i][1] < values[j][1])) {	// i dominates j
				dominates[i].push_back(j);
				dominatedCounter[j]++;
			}
		}
	}
	bool flag = true;
	int counter = 0;
	while (counter <= GA_POPSIZE) {
		vector<int> current;
		for (int i = 0; i <2* GA_POPSIZE; i++) {
			if (dominatedCounter[i] == 0) {
				current.push_back(i);
				counter++;
			} 
		}

		if (counter >= GA_POPSIZE) {
			sortByCrowding(current, values);
		}
		for (int i = 0; i < current.size(); i++) {
			result.push_back(population[current[i]]);
		}



		current.clear();
	}
	for (int i = 0; i < result.size(); i++) {
		population[i] = result[i];
	}
	createOffSpring(population,values);

}

int main()
{
	srand(unsigned(time(NULL)));
	getFileContent("C:\\Users\\omark\\source\\repos\\binPacking\\Problems\\Falkenauer_u120_00.txt");

	

	ga_vector pop_alpha, pop_beta;
	ga_vector *population, *buffer;

	init_population(pop_alpha, pop_beta);
	population = &pop_alpha;
	buffer = &pop_beta;
	int prevSol = SIZE;
	for (int i = 0; i < GA_MAXITER; i++) {
		calc_fitness(*population);		// calculate fitness
	//	fittnesShare(*population);
	//	NSGA2(*population);
		sort_by_fitness(*population);	// sort them
		localOptima(*population);
		if ((*population)[0].fitness == 0) break;
		//if (prevSol < print_best(population[0])) { // Hyper mutation 
			//Mutation_Factor++;
	//	}
		prevSol = print_best(population[0]);
		//Crowding(*population,*buffer);
		mate(*population, *buffer);		// mate the population together
		swap(population, buffer);		// swap buffers
	}
	std::cout<<print_best(*population);		// print the best one

	return 0;
}
