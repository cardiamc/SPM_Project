/**
 * Parallel and Distributed Systems: Paradigms and Models
 * Accademic Year 2019/2020
 * Final Project
 * Marco Cardia
 * Student ID: 530567
*/
#include <bits/stdc++.h>
#include <chrono>
#include <iostream>
#include <iterator>
#include <tuple>
#include <vector>

#define PRINT_TIME = 1
// #define PRINT_OVERHEAD = 1

#ifdef PRINT_TIME
#ifndef PRINT_OVERHEAD
#define PRINT_CROSSOVER_TIME = 1
#define PRINT_MUTATION_TIME = 1
#define PRINT_FITNESS_TIME = 1
#define PRINT_SELECTION_TIME = 1
#define PRINT_SHUFFLE_TIME = 1
// std::atomic<int> crossover_time = 0;
// std::atomic<int> mutation_time = 0;
// std::atomic<int> fitness_time = 0;
// std::atomic<int> selection_time = 0;
int shuffling_time = 0;

std::vector<int> *crossover_times;
std::vector<int> *mutation_times;
std::vector<int> *fitness_times;
std::vector<int> *selection_times;
#endif
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

typedef struct {
   int start;
   int end;
} RANGE;

bool insert_population(std::vector<path_t>* population, RANGE range, path_t chromosome, std::vector<path_t>* next_generation){
    if (std::find(next_generation->begin()+range.start, next_generation->begin()+range.end, chromosome) != (next_generation->begin()+range.end)) return false;

    auto max_el = std::max_element(next_generation->begin()+range.start, next_generation->begin()+range.end);

    int pos = std::distance(next_generation->begin()+range.start, max_el) + range.start;

    if(chromosome.cost <= (*max_el).cost) {
        (*next_generation)[pos] = chromosome;
        return true;
    }
    return false;
}

void selection(std::vector<path_t>& population, RANGE range, std::set<path_t> new_generation){
    for (auto new_chromosome:new_generation){
        // If the new chromosome is already present continue with the next chromosome
        // if (std::find(population.begin()+range.start, population.begin()+range.end, new_chromosome) != (population.begin()+range.end)) continue;

        auto max_el = std::max_element(population.begin()+range.start, population.begin()+range.end);

        int pos = std::distance(population.begin()+range.start, max_el) + range.start;

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
    int len = rand() % (chromos1.size()-start-2) + 2;
    
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
    RANGE range,
    float p_crossover,
    float p_mutation,
    int t_id
) {
#ifndef PRINT_OVERHEAD // Timer overhead
    std::set<path_t> new_generation;

    for (int i=range.start; i<range.end; i++) {
        path_t child1, child2;
        bool change = false;
        int next = (i+1) < range.end ? (i+1) : range.start;

        
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
            crossover_times->at(t_id) += usec_crossover;
#endif

// TIMER
#ifdef PRINT_FITNESS_TIME
            auto start_fitness_child1 = std::chrono::high_resolution_clock::now();
#endif
            // Fitness
            child2.cost = fitness(graph, (*population)[next].path);
            new_generation.insert(child2);
            change = true;

// TIMER
#ifdef PRINT_FITNESS_TIME
            auto elapsed_fitness_child1 = std::chrono::high_resolution_clock::now() - start_fitness_child1;
            auto usec_fitness_child1 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_fitness_child1).count();
            fitness_times->at(t_id) += usec_fitness_child1;
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
            auto elapsed_mutation = std::chrono::high_resolution_clock::now() - start_mutation;
            auto usec_mutation = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_mutation).count();
            mutation_times->at(t_id) += usec_mutation;
#endif
        }

        // Fitness
        if (change){ // Insert First child that can be either generated by crossover or mutated

            // TIMER
#ifdef PRINT_FITNESS_TIME
            auto start_fitness_child2 = std::chrono::high_resolution_clock::now();
#endif

            child1.cost = fitness(graph, child1.path);
            new_generation.insert(child1);

            // TIMER
#ifdef PRINT_FITNESS_TIME
            auto elapsed_fitness_child2 = std::chrono::high_resolution_clock::now() - start_fitness_child2;
            auto usec_fitness_child2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_fitness_child2).count();
            fitness_times->at(t_id) += usec_fitness_child2;
#endif
        }

    }
    // TIMER
#ifdef PRINT_SELECTION_TIME
    auto start_selection = std::chrono::high_resolution_clock::now();
#endif
    selection(*population, range, new_generation);
    
    // TIMER
#ifdef PRINT_SELECTION_TIME
    auto elapsed_selection = std::chrono::high_resolution_clock::now() - start_selection;
    auto usec_selection = std::chrono::duration_cast<std::chrono::microseconds>(elapsed_selection).count();
    selection_times->at(t_id) += usec_selection;
#endif
#endif
}