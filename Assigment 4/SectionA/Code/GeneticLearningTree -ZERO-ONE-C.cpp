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
#define MAX_SCORE 40
#define SCORE_POINTS 10
// #define OPTIMAL_NUMBER_OF_OPERATORS 4 // for XOR
// #define OPTIMAL_NUMBER_OF_OPERATORS 5 // for NOT XOR (NXOR)
#define OPTIMAL_NUMBER_OF_OPERATORS 2 // for ONE_ZERO & ZERO_ONE
// #define OPTIMAL_NUMBER_OF_OPERATORS 1 // for ZEROS_OR_ZERO_ONE
// #define OPTIMAL_NUMBER_OF_OPERATORS 0 // for ONES_OR_ONE_ZERO

#define MUTATION_RATE 7 // out of 10 when to start mutation
#define CROSSOVER_LIMIT 45 // out of 100, the percentage is 45% (0-45)
#define MUTATION_LIMIT 90 // out of 100, the percentage is 45% (45-90)
#define ELITISIM_SIZE GA_ELITRATE * GA_POPSIZE
// #define TARGET_METHOD "XOR"
// #define TARGET_METHOD "NXOR"
// #define TARGET_METHOD "ONE_ZERO"
#define TARGET_METHOD "ZERO_ONE"
// #define TARGET_METHOD "ZEROS_OR_ZERO_ONE"
// #define TARGET_METHOD "ONES_OR_ONE_ZERO"

#define FITNESS_METHOD 0 // each input indiviualy
// #define FITNESS_METHOD 1 // all inputs together


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
typedef enum Operands { A , B } OPERANDS;
typedef enum Sides { LEFT, RIGHT } SIDES;

typedef struct node_t {
	OPERATORS op;
	OPERANDS operand;
	bool isLeaf;
	struct node_t* left_child;
	struct node_t* right_child;
	
}Node;

typedef struct gene_t {
	Node* tree;
	int fitness; // score

}Gene;

typedef vector<Gene> genes_vector;

int calc_fitness(Node* node);
void print_gene_tree(Node* tree);
int calc_fitness_general(Node* node);


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

/* init_node_full: initializes a node with full method and retrurns it */
Node* init_node_full(int startDepth) {
	Node* tmpNode = (Node*)malloc(sizeof(Node));
	if (!tmpNode) {
		malloc_failed("failed to allocate memory in function init_node_full at depth = " + startDepth);
		return NULL;
	}
	if (startDepth < MAX_PARSE_TREE_DEPTH) {
		int rnd = rand() % 3;
		tmpNode->op = (OPERATORS)rnd;
		tmpNode->isLeaf = false;
		startDepth++;
		tmpNode->left_child = init_node_full(startDepth);
		if (tmpNode->op != NOT) {
			tmpNode->right_child = init_node_full(startDepth);
		}
		else {
			tmpNode->right_child = NULL;
		}
	}
	else {
		int rnd = rand() % 2;
		tmpNode->operand = (OPERANDS)rnd;
		tmpNode->isLeaf = true;
		tmpNode->left_child = NULL;
		tmpNode->right_child = NULL;
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
		int rnd = rand() % 5;
		if (rnd < 3) {
			tmpNode->op =(OPERATORS) rnd;
			tmpNode->isLeaf = false;
			startDepth++;
			tmpNode->left_child = init_node_grow(startDepth);
			if (tmpNode->op != NOT) { // NOT is an unary operator
				tmpNode->right_child = init_node_grow(startDepth);
			}
			else {
				tmpNode->right_child = NULL;
			}
		}
		else {
			tmpNode->operand = (OPERANDS)(rnd-3);
			tmpNode->isLeaf = true;
			tmpNode->left_child = NULL;
			tmpNode->right_child = NULL;
		}
		
	}
	else {
		int rnd = rand() % 2;
		tmpNode->operand = (OPERANDS)rnd;
		tmpNode->isLeaf = true;
		tmpNode->left_child = NULL;
		tmpNode->right_child = NULL;
	}
	return tmpNode;
}

/* init_pop_full: adds to the vector a <size> nodes that were generated via full method */
void init_pop_full(genes_vector& vec, int size) {
	for (int i = 0; i < size; i++)
	{
		Gene gene;
		gene.tree = init_node_full(ROOT_DEPTH);
		gene.fitness = calc_fitness_general(gene.tree);
		vec.push_back(gene);
	}
}

/* init_pop_grow: adds to the vector a <size> nodes that were generated via grow method */
void init_pop_grow(genes_vector& vec, int size) {
	for (int i = 0; i < size; i++)
	{
		Gene gene;
		gene.tree = init_node_grow(ROOT_DEPTH);
		gene.fitness = calc_fitness_general(gene.tree);
		vec.push_back(gene);
	}
}

/* init_pop_ramped_half_and_half: initialize */
void init_pop_ramped_half_and_half(genes_vector& vec) {
	init_pop_full(vec, GA_POPSIZE_HALF);
	init_pop_grow(vec, GA_POPSIZE_HALF);
}


/* test_tree: tests a tree with a specific input */
int test_tree(Node* node, int A, int B) {
	if (!node) {
		return -1; //mustn't get here
	}
	if (node->isLeaf) {
		if (node->operand == OPERANDS::A) {
			return A;
		}
		else {
			return B;
		}
	}
	// node is not a leaf
	int tmpLeft = test_tree(node->left_child, A, B);
	if (node->op == NOT) {
		return ((tmpLeft == 0) ? 1 : 0); // NOT
	}
	int tmpRight = test_tree(node->right_child, A, B);
	// AND or OR
	if (node->op == AND) {
		return (tmpLeft & tmpRight);
	}
	return (tmpLeft | tmpRight);
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

/* check_result: returns the result of the target method */
int check_result(int A, int B) {
	if (TARGET_METHOD == "XOR") {
		return A^B;
	}
	if (TARGET_METHOD == "NXOR") {
		return (A^B == 1) ? 0 : 1;
	}
	if (TARGET_METHOD == "ONE_ZERO") {
		return (A == 1 && B == 0) ? 1 : 0;
	}
	if (TARGET_METHOD == "ZERO_ONE") {
		return (A == 0 && B == 1) ? 1 : 0;
	}
	if (TARGET_METHOD == "ZEROS_OR_ZERO_ONE") {
		return (A == 0) ? 1 : 0;
	}
	if (TARGET_METHOD == "ONES_OR_ONE_ZERO") {
		return (A == 1) ? 1 : 0;
	}
	return -1;
}

/* test_tree_all_at_once: tests the genes all input options at once */
/* (A,B) = (0,0),(1,0),(0,1),(1,1) */
vector<int> test_tree_all_at_once(Node* node) {
	vector<int> vec;
	for (int i = 0; i < 4; i++)
	{
		vec.push_back(-1);
	}
	if (!node) {
		return vec;
	}
	int a = 0;
	int b = 0;
	if (node->isLeaf) {
		for (int i = 0; i < 4; i++)
		{
			//update a
			if (i % 2 == 0) {
				a = 0;
			}
			else {
				a = 1;
			}
			//update b
			if (i < 2) {
				b = 0;
			}
			else {
				b = 1;
			}
			// check the operand
			if (node->operand == OPERANDS::A) {
				vec[i] = a;
			}
			else {
				vec[i] = b;
			}
		}
		return vec;
	}
	// node is not a leaf
	vector<int> tmpLeft = test_tree_all_at_once(node->left_child);
	vector<int> tmpRight = test_tree_all_at_once(node->right_child);
	int res = 0;
	for (int i = 0; i < 4; i++)
	{
		switch (node->op) {
		case NOT: res = (tmpLeft[i] == 0) ? 1 : 0;
			break;
		case AND: res = tmpLeft[i] & tmpRight[i];
			break;
		case OR: res = tmpLeft[i] | tmpRight[i];
			break;
		default: res = -1; // mustn't get here
			break;
		}
		vec[i] = res;
	}
	return vec;
 }

/* calc_fitness_all_at_once: calculates the fitness of a gene */
int calc_fitness_all_at_once(Node* node) {
	int score = MAX_SCORE;
	if (!node) {
		return score;
	}
	vector<int> vec = test_tree_all_at_once(node);
	if (vec[0] == check_result(0, 0)) {
		score -= SCORE_POINTS;
	}
	if (vec[1] == check_result(1, 0)) {
		score -= SCORE_POINTS;
	}
	if (vec[2] == check_result(0, 1)) {
		score -= SCORE_POINTS;
	}
	if (vec[3] == check_result(1, 1)) {
		score -= SCORE_POINTS;
	}
	// we need the depth now,or the number of operators 
	int numOfOperators = calc_num_of_operators(node);
	score += abs(numOfOperators - OPTIMAL_NUMBER_OF_OPERATORS);
	return score;
}


/* calc_fitness: calculates the fitness of a gene , optimal: (NOT(A AND B)) AND ( A OR B) */
int calc_fitness(Node* node) {
	int score = MAX_SCORE;
	if (!node) {
		return score;
	}
	if (test_tree(node, 0, 0) == check_result(0, 0)) {
		score -= SCORE_POINTS;
	}
	if (test_tree(node, 1, 0) == check_result(1, 0)) {
		score -= SCORE_POINTS;
	}
	if (test_tree(node, 0, 1) == check_result(0, 1)) {
		score -= SCORE_POINTS;
	}
	if (test_tree(node, 1, 1) == check_result(1, 1)) {
		score -= SCORE_POINTS;
	}
	// we need the depth now,or the number of operators 
	int numOfOperators = calc_num_of_operators(node);
	score += abs(numOfOperators - OPTIMAL_NUMBER_OF_OPERATORS);
	return score;
}


/* calc_fitness_general: calculates the fitnees of a gene tree using a method as described in FITNESS_METHOD */
int calc_fitness_general(Node* tree) {
	if (FITNESS_METHOD == 0) {
		return calc_fitness(tree);
	}
	return calc_fitness_all_at_once(tree);
	
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
			node->operand = (node->operand == A) ? B : A; //switch between A & B
			return;
		}
		else {
			num = rand() % 3;
			OPERATORS op = OPERATORS(num);
			if (node->op != op) {
				node->op = op;
			}
			else {
				num = (num + 1) % 3;
				op = OPERATORS(num);
				node->op = op;
			}
			if (op == NOT) {
				destroyNode(node->right_child);
				node->right_child = NULL;
			}
			else if(!node->right_child) {
				int depth = (node->left_child->isLeaf) ? MAX_PARSE_TREE_DEPTH : MAX_PARSE_TREE_DEPTH - 1;
				node->right_child = init_node_full(depth);
				initedRightSubTree = true;
			}
		}
	}
	tree_mutation(node->left_child, depth + 1);
	if (!initedRightSubTree) {
		tree_mutation(node->right_child, depth + 1);
	}
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
			bool ad = tmpGene.tree->isLeaf;
			tree_mutation(tmpGene.tree);

		}
		tmpGene.fitness = calc_fitness_general(tmpGene.tree);
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
	if (tree->isLeaf) {
		st = (tree->operand == A) ? "A" : "B";
		cout << st;
		return;
	}
	if (tree->op == AND) {
		cout << "AND";
	}
	else {
		st = (tree->op == OR) ? "OR" : "NOT";
		cout << st;
	}
	cout << "(";
	print_gene_tree(tree->left_child);
	cout << ",";
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
