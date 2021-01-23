/**
 * Parallel and Distributed Systems: Paradigms and Models
 * Accademic Year 2019/2020
 * Final Project
 * Marco Cardia
 * Student ID: 530567
*/
#include <chrono>
#include "genetic_parallel.cpp"
#include <iostream>
#include <set>
#include <stdlib.h>
#include <thread>
#include <tuple>
#include "utility.cpp"
#include <vector>
#include <ff/utils.hpp>
#include <ff/ff.hpp>

// #define PRINT_CONVERGENCE = 1;
// #define PRINT_SOLUTION = 1;

using namespace std;
using namespace ff;

#define INITTIME   auto start = std::chrono::high_resolution_clock::now();\
  auto elapsed = std::chrono::high_resolution_clock::now() - start;\
  auto usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();\

#define BEGINTIME start = std::chrono::high_resolution_clock::now();

#define ENDTIME(s,nw)   elapsed = std::chrono::high_resolution_clock::now() - start; \
  usec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();\
  cout << s << "\t" << usec << " usecs with " << nw << " threads " << endl;


struct Task{
	Task(std::vector<path_t>* population, RANGE range):
		population(population), range(range){}
    
    std::vector<path_t>* population;
    RANGE range;
};

// using pair_t = std::pair<std::vector<path_t>, int>; //TODO insert best soluzion (?)

struct Emitter:ff_monode_t<int, Task> {
    Emitter(std::vector<path_t>* population, int size_population, int epoch, const int nw):
        population(population), size_population(size_population), epoch(epoch), nw(nw) {
            ranges.reserve(nw);
        }
    
    Task* svc(int *s) {
        if (s==nullptr) {
            int delta_range {size_population/nw};

            for(int i=0; i<nw; i++) {
                ranges[i].start = (i == 0 ? 0 : i*delta_range);
                ranges[i].end   = (i != (nw-1) ? (i+1)*delta_range: size_population);
                Task* task = new Task(population, ranges[i]);
                ff_send_out_to(task, i);
            }

            return GO_ON;
        }

        if (++nw_b < nw) {
            delete s;
            return GO_ON; // Join threads
        }
        
        if (--epoch == 0) {
#ifdef PRINT_CONVERGENCE
        std::cout<<"Convergency: \n";
        for(auto el:convergence){
            std::cout<<el<<" ";
        }
        std::cout<<"\n";
#endif
            return EOS;
        }

        nw_b = 0;

        // TIMER
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

#ifdef PRINT_CONVERGENCE
        convergence.push_back(std::min_element(population->begin(), population->end())->cost);
#endif
        for(int i=0; i<nw; i++) {
            Task* task = new Task(population, ranges[i]);
            ff_send_out_to(task, i);
        }

        return GO_ON;            
    }

    
    void svc_end() {
        // just for debugging
        // std::cout<<"Task terminated\n";
    }

#ifdef PRINT_CONVERGENCE
    std::vector<int> convergence; 
#endif
    const int nw;
    int size_population;
    std::vector<RANGE> ranges;
    int* pairs = nullptr; // TODO
    std::vector<path_t>* population;
    int epoch;
    int nw_b = 0;
};

struct Worker:ff_node_t<Task, int> {
    Worker(int** graph, const float p_crossover, const float p_mutation):
        graph(graph), p_crossover(p_crossover), p_mutation(p_mutation){}

    void print_population(std::vector<path_t>* population) {
        std::cout<<"The population has size: "<<population->size()<<"\n";
        for(std::vector<path_t>::const_iterator it=population->begin(); it!=population->end(); ++it){
            std::cout<<"The path is: ";
            for(auto p:(*it).path) std::cout<<p<<" ";
            std::cout<<"Costs: "<<(*it).cost<<"\n";
        }
    }

    int* svc(Task* in) {
        // auto new_state;
        genetic_algorithm(graph, (in->population), in->range, p_crossover, p_mutation);
        auto new_state = std::min_element(in->population->begin()+in->range.start, in->population->begin()+in->range.end);

        delete in;
        return new int(new_state->cost);
    }

    void svc_end() {
        // just for debugging
        // std::cout<<"Task terminated\n";
    }
    
    int **graph;
    const float p_crossover;
    const float p_mutation;
};

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

int main(int argc, char * argv[]) {
    if(argc!=6) {
        std::cout << "Usage is: " << argv[0] << " number_nodes population_size epochs seed nw" << std::endl;
        return(-1);
    }

    int n_nodes          = atoi(argv[1]);
    int population_size  = atoi(argv[2]);
    int epochs           = atoi(argv[3]);
    int seed             = atoi(argv[4]);
    int nw               = atoi(argv[5]);

    if (population_size<2) {
        std::cout << "Population size should be > 1" << std::endl;
        return(-1);
    }

    srand(seed);

    float p_crossover = 0.2, p_mutation = 0.2;

    // matrix representation of graph
    int** graph = init_matrix(n_nodes, seed);
    // print_matrix(graph, n_nodes);

    // --------------------------------

    path_t min_path;
    min_path.cost = INT_MAX;

    // Store the population
    std::vector<path_t>* population;
    // Initialise the population
    population = init_population(graph, population_size, n_nodes);
   
    Emitter emitter(population, population_size, epochs, nw);
    std::vector<std::unique_ptr<ff_node>> workers;
    for(int i=0; i<nw; ++i)
        workers.push_back(make_unique<Worker>(graph, p_crossover, p_mutation));

    ff_Farm<void> farm(std::move(workers), emitter);

    farm.remove_collector();
    farm.wrap_around();

// TIMER
#ifdef PRINT_TIME
    auto start = std::chrono::high_resolution_clock::now();
#endif
    if (farm.run_and_wait_end()<0) {
        error("running farm");
        return -1;
    }
// TIMER
#ifdef PRINT_TIME
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    auto tot_time = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
#endif

    min_path =  *(std::min_element(population->begin(), population->end()));

    // --------------------------------
#ifdef PRINT_TIME
#ifdef PRINT_CROSSOVER_TIME
    std::cout << "Crossover requires " << "\t" << crossover_time << " usecs with "<< nw <<" threads " << std::endl;
#endif
#ifdef PRINT_MUTATION_TIME
    std::cout << "Mutation requires " << "\t" << mutation_time << " usecs with "<< nw <<" threads " << std::endl;
#endif
#ifdef PRINT_FITNESS_TIME
    std::cout << "Fitness requires " << "\t" << fitness_time << " usecs with "<< nw <<" threads " << std::endl;
#endif
#ifdef PRINT_SELECTION_TIME
    std::cout << "Selection requires " << "\t" << selection_time << " usecs with "<< nw <<" threads " << std::endl;
#endif
#ifdef PRINT_SHUFFLE_TIME
    std::cout << "Sorting requires " << "\t" << shuffling_time << " usecs with "<< nw <<" threads " << std::endl;
#endif
    std::cout << "Execution took " << "\t" << tot_time << " usecs with "<< nw <<" threads " << std::endl;
#endif


#ifdef PRINT_SOLUTION
    std::cout <<"\nThe path is: ";
    for(const auto& i: min_path.path)
        std::cout << i << ' ';

    std::cout <<"\nThe cost is: " << min_path.cost << std::endl;
#endif

    delete_matrix(graph, n_nodes);
    return 0;
}