// Genetic5.cpp : Defines the entry point for the console application.
//


#pragma warning(disable:4786)		// disable debug warning

#include <iostream>					// for cout etc.
#include <vector>					// for vector class
#include <string>					// for string class
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()

#define GA_POPSIZE		30 //2048		// ga population size
#define GA_POPSIZE_HALF		15 //1024		// ga population size
#define GA_MAXITER		500//16384		// maximum iterations
//#define GA_MAXITER		1638		// maximum iterations
#define GA_ELITRATE		0.10f		// elitism rate
#define GA_MUTATIONRATE	0.25f		// mutation rate
#define GA_MUTATION		RAND_MAX * GA_MUTATIONRATE


#define MAX_PARSE_TREE_DEPTH 3 // max depth of tree
// #define MAX_PARSE_TREE_DEPTH 5 // max depth of tree
#define ROOT_DEPTH 0
#define MAX_SCORE 42
#define SCORE_POINTS 2
#define OPTIMAL_NUMBER_OF_OPERATORS 3

#define MUTATION_RATE 7 // out of 10 when to start mutation
#define CROSSOVER_LIMIT 45 // out of 100, the percentage is 45% (0-45)
#define MUTATION_LIMIT 90 // out of 100, the percentage is 45% (45-90)
#define ELITISIM_SIZE GA_ELITRATE * GA_POPSIZE

#define POINTS_SIZE 21
#define ERROR_LIMIT 0.1f


using namespace std;				// polluting global namespace, but hey...
using std::cout;
using std::endl;

struct ga_struct
{
	string str;						// the string
	unsigned int fitness;			// its fitness
	int age;						// it's age - start at 0
	unsigned int aging_fitness;			// its fitness considernig the age
};

typedef vector<ga_struct> ga_vector;// for brevity

typedef enum Operators { AND , OR , NOT} OPERATORS;
typedef enum Operands { X , NUM } OPERANDS;
typedef enum Sides { LEFT, RIGHT } SIDES;
typedef enum MathOperations { ADD, SUB, MUL, DIV } MATH_OPERATIONS;

const double POINTS [POINTS_SIZE][2] = { {-1,1},{-0.9,0.91},{-0.8,0.84},{-0.7,0.79},{-0.6,0.76},{-0.5,0.75},{-0.4,0.76},{-0.3,0.79},{-0.2,0.84},{-0.1,0.91},
								{-1.38778e-16,1},{0.1,1.11},{0.2,1.24},{0.3,1.39},{0.4,1.56},{0.5,1.75},{0.6,1.96},{0.7,2.19},{0.8,2.44},{0.9,2.71},{1,3} };

typedef struct node_t {
	MATH_OPERATIONS op;
	OPERANDS operand;
	bool isLeaf;
	struct node_t* left_child;
	struct node_t* right_child;
	double num;
	
}Node;

typedef struct gene_t {
	Node* tree;
	int fitness; // score

}Gene;

typedef vector<Gene> genes_vector;

int calc_fitness(Node* node);
void print_gene_tree(Node* tree);


/* destroyNode: destroys and frees a tree with root node */
void destroyNode(Node* node) {
	if (node == NULL) {
		return;
	}
	destroyNode(node->left_child);
	destroyNode(node->right_child);
	free(node);
}

/* destroyGene: destroys and free allocated memory for a gene */
inline void destroyGene(Gene& gene) {
	destroyNode(gene.tree);
}

/* malloc_failed: allocation fialed , report it */
void malloc_failed(const std::string& message) {
	cout << "Memory allocation failed: " << message << endl;
	cout << "The program may not function properly" << endl;
}

/* emptyGenesVector: empty's a vector from nodes*/
void emptyGenesVector(genes_vector& vec) {
	std::for_each(vec.begin(), vec.end(), destroyGene);
}

/* get_correct_num: returns the correct num for a node */
int get_correct_num(Node* node) {
	if (!node || !(node->isLeaf) || (node->isLeaf && node->operand == X)) {
		return 0;
	}
	return node->num = (rand() % 20) - 10; // between -9 & 9
}


/* init_node_full: initializes a node with full method and retrurns it */
Node* init_node_full(int startDepth) {
	Node* tmpNode = (Node*)malloc(sizeof(Node));
	if (!tmpNode) {
		malloc_failed("failed to allocate memory in function init_node_full at depth = " + startDepth);
		return NULL;
	}
	if (startDepth < MAX_PARSE_TREE_DEPTH) {
		int rnd = rand() % 4;
		tmpNode->op = (MATH_OPERATIONS)rnd;
		tmpNode->isLeaf = false;
		startDepth++;
		tmpNode->left_child = init_node_full(startDepth);
		tmpNode->right_child = init_node_full(startDepth);
		tmpNode->num = get_correct_num(tmpNode);
	}
	else {
		int rnd = rand() % 2;
		tmpNode->operand = (OPERANDS)rnd;
		tmpNode->isLeaf = true;
		tmpNode->left_child = NULL;
		tmpNode->right_child = NULL;
		tmpNode->num = get_correct_num(tmpNode);
	}
	return tmpNode;
}

/* init_node_grow: initializes a node with grow method and retrurns it */
Node* init_node_grow(int startDepth) {
	Node* tmpNode = (Node*)malloc(sizeof(Node));
	if (!tmpNode) {
		malloc_failed("failed to allocate memory in function init_node_grow at depth = " + startDepth);
		return NULL;
	}
	if (startDepth < MAX_PARSE_TREE_DEPTH) {
		int rnd = rand() % 6;
		if (rnd < 4) {
			tmpNode->op =(MATH_OPERATIONS) rnd;
			tmpNode->isLeaf = false;
			startDepth++;
			tmpNode->left_child = init_node_grow(startDepth);
			tmpNode->right_child = init_node_grow(startDepth);
			tmpNode->num = get_correct_num(tmpNode);
		}
		else {
			tmpNode->operand = (OPERANDS)(rnd-4);
			tmpNode->isLeaf = true;
			tmpNode->left_child = NULL;
			tmpNode->right_child = NULL;
			tmpNode->num = get_correct_num(tmpNode);
		}
		
	}
	else {
		int rnd = rand() % 2;
		tmpNode->operand = (OPERANDS)rnd;
		tmpNode->isLeaf = true;
		tmpNode->left_child = NULL;
		tmpNode->right_child = NULL;
		tmpNode->num = get_correct_num(tmpNode);
	}
	return tmpNode;
}

/* init_pop_full: adds to the vector a <size> nodes that were generated via full method */
void init_pop_full(genes_vector& vec, int size) {
	for (int i = 0; i < size; i++)
	{
		Gene gene;
		gene.tree = init_node_full(ROOT_DEPTH);
		gene.fitness = calc_fitness(gene.tree);
		vec.push_back(gene);
	}
}

/* init_pop_grow: adds to the vector a <size> nodes that were generated via grow method */
void init_pop_grow(genes_vector& vec, int size) {
	for (int i = 0; i < size; i++)
	{
		Gene gene;
		gene.tree = init_node_grow(ROOT_DEPTH);
		gene.fitness = calc_fitness(gene.tree);
		vec.push_back(gene);
	}
}

/* init_pop_ramped_half_and_half: initialize */
void init_pop_ramped_half_and_half(genes_vector& vec) {
	init_pop_full(vec, GA_POPSIZE_HALF);
	init_pop_grow(vec, GA_POPSIZE_HALF);
}


/* test_tree: tests a tree with a specific input */
double test_tree(Node* node, double X) {
	if (!node) {
		return -1; //mustn't get here
	}
	if (node->isLeaf) {
		if (node->operand == OPERANDS::X) {
			return X;
		}
		else {
			return node->num;
		}
	}
	// node is not a leaf
	double tmpLeft = test_tree(node->left_child, X);
	double tmpRight = test_tree(node->right_child, X);
	if (node->op == ADD) {
		return tmpLeft + tmpRight;
	}
	if (node->op == SUB) {
		return tmpLeft - tmpRight;
	}
	if (node->op == MUL) {
		return tmpLeft * tmpRight;
	}
	// div
	if (tmpRight == 0) { // devide by zero hazard!
		return 1;
	}
	return tmpLeft / tmpRight;
}

/* calc_num_of_operators: calculates and retruns the number of operators in a tree */
int calc_num_of_operators(Node* node) {
	if (!node || node->isLeaf) {
		return 0;
	}
	// node is not a leaf
	int tmpLeft = calc_num_of_operators(node->left_child);
	int tmpRight = calc_num_of_operators(node->right_child);
	return 1 + tmpLeft + tmpRight;
}

/* check_result: returns the result of the target method - X^2+X+1 */
double check_result(int X) {
	return ((X * X) + X + 1);
}

/* calc_fitness: calculates the fitness of a gene , optimal: (X*X)+X+1 */
int calc_fitness(Node* node) {
	int score = MAX_SCORE;
	if (!node) {
		return score;
	}
	double res = 0;
	for (int i = 0; i < POINTS_SIZE; i++) {
		res = test_tree(node, POINTS[i][0]);
		if (abs(res - POINTS[i][1]) <= ERROR_LIMIT) {
			score -= SCORE_POINTS;
		}
	}
	// we need the depth now,or the number of operators 
	int numOfOperators = calc_num_of_operators(node);
	score += abs(numOfOperators - OPTIMAL_NUMBER_OF_OPERATORS);
	return score;
}

/* tree_get_parent_of_replaced_node: returns a the parent node of one to replace to replace  */
Node* tree_get_parent_of_replaced_node(Node* tree,SIDES& side,int actualDepth,int& nodeDepth,int& maxRndNum,bool getLeaf,bool useMinimumDepth = false,int minDepth = 0) {
	if (!tree) {
		maxRndNum = -1;// to not be chosen later
		return NULL;
	}
	if (tree->isLeaf || (!tree->left_child && !tree->right_child)) { // better not get here
		maxRndNum = rand() % 100;
		nodeDepth = actualDepth;
		return tree; //for now this is the best option
	}
	
	
	int tmpRndLeft = rand() % 100;
	int tmpRndRight = rand() % 100;

	//children are null or leaves
	if (tree->left_child && tree->left_child->isLeaf && !(tree->right_child)) {
		// left is leaf,right is null
		if (!getLeaf) {
			maxRndNum = -1; // to choose something else
		}
		else {
			maxRndNum = tmpRndLeft;
		}
		side = LEFT;
		nodeDepth = actualDepth + 1;
		return tree;// this is the parent
	}
	if (tree->right_child && tree->right_child->isLeaf && !(tree->left_child)) {
		// right is leaf,left is null
		if (!getLeaf) {
			maxRndNum = -1; // to choose something else
		}
		else {
			maxRndNum = tmpRndRight;
		}
		side = RIGHT;
		nodeDepth = actualDepth + 1;
		return tree;// this is the parent
	}
	if (tree->right_child && tree->right_child->isLeaf && tree->left_child && tree->left_child->isLeaf) {
		// both are leaves, choose the max
		if (!getLeaf) {
			maxRndNum = -1; // to choose something else
		}
		else if (tmpRndRight > tmpRndLeft) {
			side = RIGHT;
			maxRndNum = tmpRndRight;
		}
		else {
			side = LEFT;
			maxRndNum = tmpRndLeft;
		}
		nodeDepth = actualDepth + 1;
		return tree; // the parent
	}
	// can be chosen and children are internal
	Node* tmpParentLeft = tree_get_parent_of_replaced_node(tree->left_child, side, actualDepth + 1, nodeDepth, maxRndNum, getLeaf, useMinimumDepth, minDepth);
	SIDES tmpSide = side;
	int tmpMaxRndNum = maxRndNum;
	int tmpNodeDepth = nodeDepth;
	Node* tmpParentRight = tree_get_parent_of_replaced_node(tree->right_child, side, actualDepth + 1, nodeDepth, maxRndNum, getLeaf, useMinimumDepth, minDepth);
	Node* tmpParent = tmpParentRight;
	if (!tmpParent) {
		tmpParent = tmpParentLeft;// to solve NULL issue
	}
	if (tmpMaxRndNum > maxRndNum) {
		side = tmpSide;
		maxRndNum = tmpMaxRndNum;
		nodeDepth = tmpNodeDepth;
		tmpParent = tmpParentLeft;
	}
	if (!getLeaf && !(useMinimumDepth && actualDepth+1 < minDepth)) { // can & should be an internal node at the next depth
		if (tmpRndLeft > maxRndNum && tmpRndLeft >= tmpRndRight) {
			side = LEFT;
			maxRndNum = tmpRndLeft;
			nodeDepth = actualDepth + 1;
			tmpParent = tree;
		}
		if (tmpRndRight > maxRndNum && tmpRndRight >= tmpRndLeft) {
			side = RIGHT;
			maxRndNum = tmpRndRight;
			nodeDepth = actualDepth + 1;
			tmpParent = tree;
		}
	}
	return tmpParent;

}

/* copy_node: creates and returns a copy of a node and it's depth in nodeDepth - starts at 0 */
Node* copy_node(Node* node, int& nodeDepth, int depth = 0) {
	if (!node) {
		return NULL;
	}
	Node* tmpNode = (Node*)malloc(sizeof(Node));
	if (!tmpNode) {
		malloc_failed("failed to allocate memory in function copy_node at depth = " + depth);
		return NULL;
	}
	tmpNode->isLeaf = node->isLeaf;
	tmpNode->op = node->op;
	tmpNode->operand = node->operand;
	tmpNode->num = node->num;
	tmpNode->left_child = copy_node(node->left_child, nodeDepth, depth + 1);
	tmpNode->right_child = copy_node(node->right_child, nodeDepth, depth + 1);
	if (depth > nodeDepth) {
		nodeDepth = depth;
	}
	return tmpNode;
}


/* get_gene_to_crossover_with: returns a random gene to crossover with from vec */
Gene get_gene_to_crossover_with(genes_vector& vec) {
	int size = vec.size();
	int num = rand() % size;
	return vec[num];
}

/* tree_crossover: takes a gene and crossovers it with another one */
void tree_crossover(Gene& gene, genes_vector& vec) {

	SIDES side1 = LEFT;
	SIDES side2 = LEFT;
	int depth1 = 0;
	int maxRnd = rand() % 100;
	bool getLeaf = false;
	if (maxRnd >= 90) {
		getLeaf = true;
	}
	Node* parent1 = tree_get_parent_of_replaced_node(gene.tree, side1,0, depth1, maxRnd, getLeaf);
	// get gene!
	Gene gene2 = get_gene_to_crossover_with(vec);//maybe it's the same?
	int depth2 = 0;
	Node* parent2 = tree_get_parent_of_replaced_node(gene2.tree, side2,0,depth2, maxRnd,false, true, depth1); // check depth1
	// crossover - deep copy
	Node* copiedNode = NULL;
	Node* nodeToCopy = parent2;	
	if (side2 == LEFT) {
		if (parent2->left_child) { // To solve null issue
			nodeToCopy = parent2->left_child;
		}
		copiedNode = copy_node(nodeToCopy, depth2);
	}
	else {
		if (parent2->right_child) { // To solve null issue
			nodeToCopy = parent2->right_child;
		}
		copiedNode = copy_node(nodeToCopy, depth2);
	}
	if (side1 == LEFT) {
		destroyNode(parent1->left_child);
		parent1->left_child = copiedNode;
	}
	else {
		destroyNode(parent1->right_child);
		parent1->right_child = copiedNode;
	}
}


/* tree_mutation: mutates a tree/node */
void tree_mutation(Node* node,int depth =0) {
	if (!node) {
		return;
	}
	int num = rand() % 10;
	bool initedRightSubTree = false;
	if (num >= MUTATION_RATE) { //change a few of them
		if (node->isLeaf || (!node->left_child && !node->right_child)) {
			node->operand = (node->operand == X) ? NUM : X; //switch between X & NUM
			node->num = get_correct_num(node);
		}
		else {
			num = rand() % 4;
			MATH_OPERATIONS op = MATH_OPERATIONS(num);
			if (node->op != op) {
				node->op = op;
			}
			else {
				num = (num + 1) % 4;
				op = MATH_OPERATIONS(num);
				node->op = op;
			}
			node->num = get_correct_num(node);
		}
	}
	tree_mutation(node->left_child, depth + 1);
	tree_mutation(node->right_child, depth + 1);
}

bool tree_fitness_sort(Gene x, Gene y)
{
	return (x.fitness < y.fitness);
}

inline void tree_sort_by_fitness(genes_vector &population)
{
	sort(population.begin(), population.end(), tree_fitness_sort);
}

/* copy_elitism: keep the elitism alive for the next generation */
void copy_elitism(genes_vector& sourceVec, genes_vector& destVec) {
	//make sure they are sorted
	tree_sort_by_fitness(sourceVec);
	int tmpDepth = 0;
	for (int i = 0; i < ELITISIM_SIZE; i++)
	{
		Gene gene;
		gene.fitness = sourceVec[i].fitness;
		gene.tree = copy_node(sourceVec[i].tree, tmpDepth);
		destVec.push_back(gene);
	}
}

/* pop_create_next_generation: gets next generation */
void pop_create_next_generation(genes_vector& vec) {
	genes_vector tmpVec;
	Gene tmpGene;
	int nodeDepth = 0;
	int rnd = 0;
	copy_elitism(vec, tmpVec);
	for (int i = ELITISIM_SIZE; i < GA_POPSIZE; i++)
	{
		Gene tmpGene;
		tmpGene.fitness = vec[i].fitness;
		tmpGene.tree = copy_node(vec[i].tree, nodeDepth);
		// choose method
		rnd = rand() % 100;
		if (rnd < CROSSOVER_LIMIT) {
			tree_crossover(tmpGene, vec);
		}
		else if (rnd < MUTATION_LIMIT) {
			tree_mutation(tmpGene.tree);
		}
		tmpGene.fitness = calc_fitness(tmpGene.tree);
		// else automatic reproduction
		tmpVec.push_back(tmpGene);
	}
	for (int i = 0; i < GA_POPSIZE; i++)
	{
		destroyGene(vec[i]);
		vec[i] = tmpVec[i]; // shallow copy of pointers
		//don't destroy anything from tmpVec
	}
	tree_sort_by_fitness(vec);
}


/* print_gene_tree: prints the tree of a gene as a 1 line , must print endl in the caller function */
void print_gene_tree(Node* tree) {
	if (!tree) {
		return;
	}
	std::string st = "";
	std::string stnum = to_string(tree->num);
	if (tree->isLeaf) {
		st = (tree->operand == X) ? "X" : stnum;
		cout << st;
		return;
	}
	cout << "(";
	print_gene_tree(tree->left_child);
	switch (tree->op) {
		case ADD: st = "+";
			break;
		case SUB: st = "-";
			break;
		case MUL: st = "*";
			break;
		case DIV: st = "/";
			break;
		default: st = "?";
			break;
	}
	cout << st;
	print_gene_tree(tree->right_child);
	cout << ")";
}

/* print_all_genes: prints the genes*/
void print_all_genes(genes_vector& vec) {
	cout << "Printing all genes" << endl;
	for (int i = 0; i < GA_POPSIZE; i++)
	{
		cout << "fitness: " << vec[i].fitness << " tree: ";
		print_gene_tree(vec[i].tree);
		cout << endl;
	}
}

/* destroy_vector: empties all allocated memory in a vector */
void destroy_vector(genes_vector& vec) {
	for (int i = 0; i < GA_POPSIZE; i++)
	{
		destroyGene(vec[i]);
	}
}

//============== Main ==============
int main()
{

	bool solved = false;

	srand(unsigned(time(NULL)));

	genes_vector pop_alpha;
	genes_vector *population, *buffer;

	init_pop_ramped_half_and_half(pop_alpha);
	population = &pop_alpha;

	clock_t startTimeStamp = clock();
	clock_t prevTimeStamp = startTimeStamp;
	cout << "starting with genes:" << endl << endl;
	print_all_genes(*population);
	for (int i=0; i<GA_MAXITER; i++) {
		 
		//do something here...print something !
		// maybe use a test set !
		// print_all_genes(*population);

		if ((*population)[0].fitness == 0) { // it will reach 0 fitness as always because it has no idea what is it
			clock_t elapsedTime = clock();
			elapsedTime = elapsedTime - startTimeStamp;
			cout << endl;
			cout << "Reached a local optima on generation " << i << " with time: " << elapsedTime << " clock ticks  = " << (((float)elapsedTime) / CLOCKS_PER_SEC) << " seconds" << endl;
			solved = true;
			cout << "Optimum solution: ";
			print_gene_tree((*population)[0].tree);
			cout << endl << endl;
			break;
		}
		pop_create_next_generation(*population);
	}
	cout << endl;
	cout << "ending with genes:" << endl << endl;
	print_all_genes(*population);
	destroy_vector(*population);
	if (!solved) {
		clock_t elapsedTime = clock();
		elapsedTime = elapsedTime - startTimeStamp;
		cout << endl;
		cout<<"Failed to find the solution after time: " << elapsedTime << " clock ticks  = " << (((float)elapsedTime) / CLOCKS_PER_SEC) << " seconds" << endl;
	}
	return 0;
}
