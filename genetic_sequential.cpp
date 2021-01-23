/**
 * Parallel and Distributed Systems: Paradigms and Models
 * Accademic Year 2019/2020
 * Final Project
 * Marco Cardia
 * Student ID: 530567
*/
#include <bits/stdc++.h>
#include <iostream>
#include <iterator>
#include <tuple>
#include <vector>

#define PRINT_TIME = 1

#ifdef PRINT_TIME
#define PRINT_CROSSOVER_TIME = 1
#define PRINT_MUTATION_TIME = 1
#define PRINT_FITNESS_TIME = 1
#define PRINT_SELECTION_TIME = 1
#define PRINT_SHUFFLE_TIME = 1

int crossover_time = 0;
int mutation_time = 0;
int fitness_time = 0;
int selection_time = 0;
int shuffling_time = 0;
#endif

typedef struct _path_t{
    int cost;
    std::vector<int> path;

    _path_t() : cost(INT_MAX), path({}) {}

    bool operator <(const _path_t& solution) const
    {   if (cost == solution.cost)   
            return (path < solution.path);
        return (cost < solution.cost);
    }
    bool operator ==(const _path_t& solution) const {
        return (path == solution.path);
    }

    void print() const{
        std::cout<<"The path is: ";
        for(auto p:path) std::cout<<p<<" ";
        std::cout<<"Costs: "<<cost<<"\n";
    }
} path_t;

void selection(std::vector<path_t>& population, std::set<path_t>* new_generation){
    for (auto new_chromosome:*new_generation){
        // If the new chromosome is already present continue with the next chromosome
        if (std::find(population.begin(), population.end(), new_chromosome) != (population.end())) continue;

        auto max_el = std::max_element(population.begin(), population.end());

        int pos = std::distance(population.begin(), max_el);

        if(new_chromosome.cost <= (*max_el).cost) {
            population[pos] = new_chromosome;
        } else{ // There is no any other chromosome having a lower cost
            break;
        }
    }
}

int fitness(int ** graph, std::vector<int>& path) {
   int current_pathweight = 0;
   int k = path[0];
   for (int i = 1; i < path.size(); i++) {
      if (graph[k][path[i]] == 0) {
         return INT_MAX;
      }
      current_pathweight += graph[k][path[i]];
      k = path[i];
   }

   if (graph[k][path[0]] == 0) {
      return INT_MAX;
   }
   current_pathweight += graph[k][path[0]];

   return current_pathweight;
}

std::vector<int> create_child(std::vector<int>& chromos, std::vector<int> chromos2, int start, int len){
   int chromo_len = chromos.size();
   std::vector<bool> mask(chromo_len);
   std::vector<int> child;
   for(int i=0; i<chromo_len; i++){
      mask[i] = false;
   }

   // Coping the first part
   // Marking genes already present
   for(int i=0; i<start; i++){
      child.push_back(chromos[i]);
      mask[chromos[i]] = true;
   }
   
   // Marking genes present in the end of the chromosome
   for(int i=start+len; i<chromo_len; i++){
      mask[chromos[i]] = true;
   }

   for(int i=start; i<start+len; i++){
      if(!mask[chromos2[i]]){
         child.push_back(chromos2[i]);
         mask[chromos2[i]] = true;
      } else{
         for(int j=0; j<chromo_len; j++){
            if (!mask[chromos2[j]]) {
               child.push_back(chromos2[j]);
               mask[chromos2[j]] = true;
               break;
            }
         }  
      }
   }
   
   // Coping the last part
   for(int i=start+len; i<chromo_len; i++){
      child.push_back(chromos[i]);
   }

   return child;
}

std::tuple<std::vector<int>, std::vector<int>> crossover(
    std::vector<int> chromos1, 
    std::vector<int> chromos2
) {
    std::vector<int> child1, child2;

    int start = rand() % (chromos1.size()-2);
    int len = rand() % (chromos1.size()-start-2) + 2; // Min len 2
    
    child1 = create_child(chromos1, chromos2, start, len);
    child2 = create_child(chromos2, chromos1, start, len);

    return std::make_tuple(child1, child2);
}

std::vector<int> mutation(std::vector<int> chromos){
   int pos1, pos2;
   std::vector<int> child(chromos);
   pos1 = rand() % chromos.size();
   pos2 = rand() % chromos.size();

   child[pos1] = chromos[pos2];
   child[pos2] = chromos[pos1];

   return child;
}

void genetic_algorithm (
    int ** graph, 
    std::vector<path_t> *population,
    float p_crossover,
    float p_mutation
) {
    std::set<path_t>* new_generation = new std::set<path_t>;
    for (int i=0; i<population->size(); i++) {
        path_t child1, child2;
        bool change = false;
        int next = (i+1) < population->size() ? (i+1) : 0;

        // Crossover with probability p1 
        int p1 = rand() % 100;
        if( (float) p1/100 < p_crossover){
            // TIMER
#ifdef PRINT_CROSSOVER_TIME
            auto start_crossover = std::chrono::high_resolution_clock::now();
#endif
            std::tie(child1.path, child2.path) = crossover((*population)[i].path, (*population)[next].path);
            // TIMER
#ifdef PRINT_CROSSOVER_TIME
            auto elapsed_crossover = std::chrono::high_resolution_clock::now() - start_crossover;
            auto usec_crossover = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_crossover).count();
            crossover_time += usec_crossover;
#endif


            // TIMER
#ifdef PRINT_FITNESS_TIME
            auto start_fitness_child2 = std::chrono::high_resolution_clock::now();
#endif
            // Fitness
            child2.cost = fitness(graph, (*population)[next].path);
            new_generation->insert(child2);
            change = true;

            // TIMER
#ifdef PRINT_FITNESS_TIME
            auto elapsed_fitness_child2 = std::chrono::high_resolution_clock::now() - start_fitness_child2;
            auto usec_fitness_child2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_fitness_child2).count();
            fitness_time += usec_fitness_child2;
#endif
        }
        
        // Mutation with probability p2
        int p2 = rand() % 100;
        if( (float) p2/100 < p_mutation){
            // TIMER
#ifdef PRINT_MUTATION_TIME
            auto start_mutation = std::chrono::high_resolution_clock::now();
#endif
            child1.path = !change ? mutation((*population)[i].path) : mutation(child1.path);
            change = true;
            
            // TIMER
#ifdef PRINT_MUTATION_TIME
            auto elapsed = std::chrono::high_resolution_clock::now() - start_mutation;
            auto usec_mutation = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            mutation_time += usec_mutation;
#endif
        }

        // Fitness
        if (change){
            
            // TIMER
#ifdef PRINT_FITNESS_TIME
            auto start_fitness = std::chrono::high_resolution_clock::now();
#endif
            child1.cost = fitness(graph, (*population)[i].path);
            new_generation->insert(child1);

            // TIMER
#ifdef PRINT_FITNESS_TIME
            auto elapsed_fitness = std::chrono::high_resolution_clock::now() - start_fitness;
            auto usec_fitness = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_fitness).count();
            fitness_time += usec_fitness;
#endif
        }
    }
    // TIMER
#ifdef PRINT_SELECTION_TIME
    auto start_selection = std::chrono::high_resolution_clock::now();
#endif
    selection(*population, new_generation);

    // TIMER
#ifdef PRINT_SELECTION_TIME
    auto elapsed_selection = std::chrono::high_resolution_clock::now() - start_selection;
    auto usec_selection = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_selection).count();
    selection_time += usec_selection;
#endif

    delete new_generation;
}