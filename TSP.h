#ifndef TSP_H
#define TSP_H
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <climits>
#include <random>

struct XY {
  double x;
  double y;
};

double dist(const XY, const XY);
int min(std::vector<double>, std::vector<bool>);

struct feasible_city {
  double x;
  double y;
  int number;
};

class Genetic {
 public:
  Genetic(std::string filename = "cities.csv");
  void setFile(std::string filename = "cities.csv");
  void geneticAlgorithm();
  double calcPathSize(std::vector<int>);
  void calcAllSizes();
  void normalizeSizes();
  std::vector<int> generatePath(int size = 1000);
  void generateSolutions(int count = 30);
  std::vector<int> rouletteWheel();
  std::vector<int> tournament(int participants = 20);
  std::vector<std::vector<int>> PMcrossover(std::vector<std::vector<int>> parents, int child_count = 15);
  void mutation_swap(std::vector<std::vector<int>>& children);
  void mutation_scramble(std::vector<std::vector<int>>& children);
  void nextGeneration(std::vector<std::vector<int>> children);

 private:
  std::string filename;
  std::vector<int> best_path;
  double best_size = INT_MAX;
  std::vector<std::vector<int>> paths;
  std::vector<double> path_sizes;
  std::vector<XY> coord;
};

struct Road {
  double pheromone;
  double delta_pheromone;
  bool was;
};

const double Q = 1;
const double evaporation_rate = 0.5;
const double beta = 5;

class ACO {
 public:
  ACO(std::string filename = "cities.csv", int read_count = 1000, int byte_shift = 10);
  void setFile(std::string filename = "cities.csv");
  void antColonyOptimization(int iterations = 10);
  std::vector<int> findAntWay(int size = 1000);
  void calcRoadPheromone(int from, int to);
  void calcFullPheromone(std::vector<int>&, double);
  double calcDistance(int from, int to);
  int nextCity(int from);
  double calcTourDist(std::vector<int>& tour);

 private:
  double count;
  std::vector<std::vector<Road>> roads;
  std::string filename;
  std::vector<int> best_tour;
  double best_size = INT_MAX;
  std::vector<std::vector<int>> ant_ways;
  std::vector<double> distances;
  std::vector<bool> visited;
  std::vector<XY> coord;
  // void setRoadsDef();
};
#endif // TSP_H
