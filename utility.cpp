/**
 * Parallel and Distributed Systems: Paradigms and Models
 * Accademic Year 2019/2020
 * Final Project
 * Marco Cardia
 * Student ID: 530567
*/
#include <algorithm>    // std::random_shuffle
#include <bits/stdc++.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

#define MAX_DIST 50

int** init_matrix(int size, int seed) {
    int ** graph;
    graph = new int *[size];

    for(int i=0; i<size; i++)
        graph[i] = new int[size];

    int v;
    for(int i=0; i<size; i++){
        for(int j=i; j<size; j++){
            v = rand() % MAX_DIST;
            graph[i][j] = v;
            graph[j][i] = v;
        }
    }
    for(int i=0; i<size; i++)
        graph[i][i] = 0;

    return graph;
    }

    void print_matrix(int **graph, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << graph[i][j] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "-----------------------------------------------------" << std::endl;
}

void delete_matrix(int** matrix, int size){
    for(int i=0; i<size; i++)
        delete[] matrix[i];

    delete[] matrix;
}
