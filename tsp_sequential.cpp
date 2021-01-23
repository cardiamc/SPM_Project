/**
 * Parallel and Distributed Systems: Paradigms and Models
 * Accademic Year 2019/2020
 * Final Project
 * Marco Cardia
 * Student ID: 530567
*/
// #define PRINT_CONVERGENCE = 1;
// #define PRINT_SOLUTION = 1;

#include <chrono>
#include "genetic_sequential.cpp"
#include <iostream>
#include <set>
#include <stdlib.h>
#include <thread>
#include <tuple>
#include "utility.cpp"
#include <vector>

using namespace std;

// #define INITTIME   auto start = std::chrono::high_resolution_clock::now();\
//   auto elapsed = std::chrono::high_resolution_clock::now() - start;\
//   auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();\

// #define BEGINTIME start = std::chrono::high_resolution_clock::now();

// #define ENDTIME(s,nw)   elapsed = std::chrono::high_resolution_clock::now() - start; \
//   usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();\
//   cout << s << "\t" << usec << " usecs with " << nw << " threads " << endl;

void print_population(std::vector<path_t>* population) {
    std::cout<<"The population has size: "<<population->size()<<"\n";
    for(std::vector<path_t>::const_iterator it=population->begin(); it!=population->end(); ++it){
        std::cout<<"The path is: ";
        for(auto p:(*it).path) std::cout<<p<<" ";
        std::cout<<"Costs: "<<(*it).cost<<"\n";
    }
}

std::pair<int, bool> insert_population_empty(std::vector<path_t>* population, path_t chromosome){
    auto lower = std::lower_bound(population->begin(), population->end(), chromosome);
    int pos = 0;
    if (lower != population->end()) {
        pos = std::distance(population->begin(), lower);
        if (lower->path == chromosome.path) return make_pair(pos, false);
        population->insert(population->begin()+pos, chromosome);
    } else
        population->push_back(chromosome);
        
    return make_pair(pos, true);
}

std::vector<path_t>* init_population(int ** graph, int size_population, int n_nodes) {
    std::vector<path_t>* population = new std::vector<path_t>;

    for(int i=0; i<size_population; i++) {
        std::vector<int> path(n_nodes);
        path_t chromosome;

        // Randomly initialiaze the path
        std::iota(std::begin(path), std::end(path), 0);
        std::random_shuffle(path.begin(), path.end());

        chromosome.cost = fitness(graph, path);
        chromosome.path = path;
        if (chromosome.cost == INT_MAX) i--;
        else {
            if (!insert_population_empty(population, chromosome).second) i--;
        }
   }
   return population;
}

path_t travellingSalesmanSolver(int ** graph, int n_nodes, int size_population, int epochs) {
    path_t min_path;
    min_path.cost = INT_MAX;
    float p_crossover = 0.2, p_mutation = 0.2;

    // Initialise the population
    std::vector<path_t>* population;
    population = init_population(graph, size_population, n_nodes);

    // TIMER
#ifdef PRINT_TIME
            auto start = std::chrono::high_resolution_clock::now();
#endif
    for(int i=0; i<epochs; i++) {
        genetic_algorithm(graph, population, p_crossover, p_mutation);

#ifdef PRINT_SHUFFLE_TIME
        auto start_shuffle = std::chrono::high_resolution_clock::now();
#endif

        std::random_shuffle(population->begin(), population->end());

        // TIMER
#ifdef PRINT_SHUFFLE_TIME
        auto elapsed_shuffle = std::chrono::high_resolution_clock::now() - start_shuffle;
        auto usec_shuffle = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_shuffle).count();
        shuffling_time += usec_shuffle;
#endif
    }

#ifdef PRINT_TIME
    // TIMER
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto tot_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
#ifdef PRINT_CROSSOVER_TIME
    std::cout << "Crossover requires " << "\t" << crossover_time << " usecs" << std::endl;
#endif
#ifdef PRINT_MUTATION_TIME
    std::cout << "Mutation requires " << "\t" << mutation_time << " usecs" << std::endl;
#endif
#ifdef PRINT_FITNESS_TIME
    std::cout << "Fitness requires " << "\t" << fitness_time << " usecs" << std::endl;
#endif
#ifdef PRINT_SELECTION_TIME
    std::cout << "Selection requires " << "\t" << selection_time << " usecs" << std::endl;
#endif
#ifdef PRINT_SHUFFLE_TIME
    std::cout << "Sorting requires " << "\t" << shuffling_time << " usecs" << std::endl;
#endif
    std::cout << "Execution took " << "\t" << tot_time << " usecs " << std::endl;
#endif 

#ifdef PRINT_CONVERGENCE
        std::cout<<"Convergency: \n";
        for(auto el:convergence){
            std::cout<<el<<" ";
        }
        std::cout<<"\n";
#endif

    return *(std::min_element(population->begin(), population->end()));
}

int main(int argc, char * argv[]) {
   if(argc!=5) {
      std::cout << "Usage is: " << argv[0] << " number_nodes size_population epochs seed" << std::endl;
      return(-1);
   }

   int n_nodes          = atoi(argv[1]);
   int population_size  = atoi(argv[2]);
   int epochs           = atoi(argv[3]);
   int seed             = atoi(argv[4]);

   if (population_size<2) {
      std::cout << "Population size should be > 1" << std::endl;
      return(-1);
   }

   srand(seed);

   int** graph = init_matrix(n_nodes, seed);

   // matrix representation of graph
   // print_matrix(graph, n_nodes);

   path_t min_path = travellingSalesmanSolver(graph, n_nodes, population_size, epochs);
   
#ifdef PRINT_SOLUTION
   std::cout <<"\nThe path is: ";
   for(const auto& i: min_path.path)
      std::cout << i << ' ';

   std::cout <<"\nThe cost is: " << min_path.cost << std::endl;
#endif

   delete_matrix(graph, n_nodes);
   return 0;
}