#include <ctime>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "TSP.h"

class WrongParentException {};

double dist (const XY first, const XY second){
  return sqrt(pow(second.x - first.x, 2) + pow(second.y - first.y, 2));
}

double Genetic::calcPathSize(std::vector<int> path) {
  double&& path_size = 0;
  for (int i = 0; i < path.size() - 1;i++) {// 0 - 999 path[i] 0 - 999 coord 0 - 999
    path_size += dist(this->coord[path[i]], this->coord[path[i+1]]);
  }
  return path_size;
}

void Genetic::calcAllSizes() {
  this->path_sizes.clear();
  for (size_t i = 0; i < this->paths.size(); i++) {
    double path_size = this->calcPathSize(this->paths[i]);
    this->path_sizes.push_back(path_size);
    if (path_size < this->best_size) {
      this->best_size = path_size;
      this->best_path = this->paths[i];
      std::cout << std::fixed << this->best_size << '\n';
      for (size_t i = 0; i < this->best_path.size(); i++) {
        std::cout << this->best_path[i] << ' ';
      }
      std::cout << "\n";
    } else {
      std::cout << ".\n";
    }
  }
}

void Genetic::generateSolutions(int count) {
  std::vector<int> path;
  for (int i = 0; i < count; ++i) {
    path = this->generatePath();
    this->paths.push_back(path);
  }
}

std::vector<int> Genetic::rouletteWheel() {
  int size = this->paths.size();
  std::random_device rd;
  std::mt19937 mersenne(rd());
  std::vector<int> fitness(size);
  std::fill(fitness.begin(), fitness.end(), 1);
  std::vector<int> scale(size + 1);

  for (size_t i = 0; i < this->paths.size() - 1; i++) {
    for (size_t j = i + 1; j < this->paths.size(); j++) {
      if (this->path_sizes[i] < this->path_sizes[j]) {
        fitness[i]++;
      } else if (this->path_sizes[i] > this->path_sizes[j]){
        fitness[j]++;
      }
    }
  }
  scale[0] = 0;
  for (size_t i = 1; i < scale.size(); i++) {
    scale[i] = fitness[i-1] + scale[i-1];
  }
  int sum = scale.back();
  int choice = mersenne() % sum;
  for (size_t i = 0; i < scale.size() - 1; i++) {
    if (choice >= scale[i] && choice < scale[i + 1]) {
      return this->paths[i];
    }
  }
}

std::vector<int> Genetic::tournament(int participants_count) {
  std::random_device rd;
  std::mt19937 mersenne(rd());
  std::vector<std::vector<int>> participants;
  std::vector<double> participant_results;
  std::vector<int> winner;
  double winner_results = INT_MAX;
  for (size_t i = 0; i < participants_count; i++) {
    int random_index = mersenne() % this->paths.size();
    participants.push_back(this->paths[random_index]);
    participant_results.push_back(this->path_sizes[random_index]);
  }
  for (size_t i = 0; i < participants.size(); i++) {
    if (participant_results[i] < winner_results) {
      winner_results = participant_results[i];
      winner = participants[i];
    }
  }
  return winner;
}

std::vector<int> createChild(int random_index, std::vector<std::vector<int>> parents, short parent_id) {
  if (parent_id != 0 && parent_id != 1) throw WrongParentException();
  std::vector<int> child;
  child = parents[parent_id];
  for (size_t j = 1; j < random_index; j++) {
    int k = 0;
    while (child[k] != parents[1 - parent_id][j]) {// 1 - id gives 0 when 1 and conversely
      ++k;
    };
    std::swap(child[k], child[j]);
  }
  return child;
}

std::vector<std::vector<int>> Genetic::PMcrossover(std::vector<std::vector<int>> parents, int child_count) {
  //Partially-mapped crossover
  std::random_device rd;
  std::mt19937 mersenne(rd());
  std::vector<std::vector<int>> children;
  std::vector<int> child;
  int random_index;

  for (size_t i = 0; i < child_count; i++) {
    random_index = 2 + mersenne() % (parents[0].size() - 2); //without the start and end cities
    if (i % 2 == 0) {
      child = createChild(random_index, parents, 0);
    } else {
      child = createChild(random_index, parents, 1);
    }
    children.push_back(child);
  }
  return children;
}

void Genetic::nextGeneration(std::vector<std::vector<int>> children) {
  int worst_solution_ind = 0;
  double worst_solution_size = 0;
  for (size_t i = 0; i < children.size(); i++) {
    for (size_t j = 0; j < this->paths.size(); j++) {
      if (this->path_sizes[j] > worst_solution_size) {
        worst_solution_ind = j;
      }
    }
    this->paths.erase(this->paths.begin() + worst_solution_ind);
    this->paths.push_back(children[i]);
  }
}

void Genetic::mutation_swap(std::vector<std::vector<int>>& children) {
  std::random_device rd;
  std::mt19937 mersenne(rd());
  for (size_t i = 0; i < children.size(); i++) {
    for (size_t j = 0; j < children[i].size(); j++) {
      if (mersenne() % abs((int)(this->paths.size() - this->best_path.size())) == 0) {
        int random_first = 1 + mersenne() % (children[i].size() - 1);
        int random_second = 1 + mersenne() % (children[i].size() - 1);
        while (random_second == random_first) random_second = 1 + mersenne() % (children[i].size() - 1);
        std::swap(children[i][random_first], children[i][random_second]);
      }
    }
  }
}

void Genetic::mutation_scramble(std::vector<std::vector<int>>& children) {
  std::random_device rd;
  std::mt19937 mersenne(rd());
  for (size_t i = 0; i < children.size(); i++) {
    for (size_t j = 0; j < children[i].size(); j++) {
      if (mersenne() % abs((int)(this->paths.size() - this->best_path.size())) == 0) {
        int random_first = 1 + mersenne() % (children[i].size() - 2);
        int random_second = random_first + 1 + mersenne() % (children[i].size() - random_first);
        std::shuffle(children[i].begin() + random_first, children[i].begin() + random_second, mersenne);
      }
    }
  }
}

std::vector<int> Genetic::generatePath(int size) {
  std::random_device rd;
  std::mt19937 mersenne(rd());
  std::vector<int> path;
  for (int i = 0; i < size; ++i) {
    path.push_back(i);
  }
  std::shuffle(path.begin(), path.end(), mersenne);
  path.push_back(path[0]);
  return path;
}

void Genetic::geneticAlgorithm() {
  std::random_device rd;
  std::mt19937 mersenne(rd());
  std::vector<std::vector<int>> parents;
  std::vector<std::vector<int>> children;
  this->generateSolutions();
  this->calcAllSizes();
  while (true) {
    std::cout.precision(10);
    for (size_t i = 0; i < 2; i++) {
      //parents.push_back(this->rouletteWheel());
      parents.push_back(this->tournament());
      /*std::vector<int> parent {5,7,1,3,6,4,2, 5};
      std::vector<int> parent2 {4,6,2,7,3,1,5, 4};
      parents.push_back(parent);
      parents.push_back(parent2);*/
    }
    children = this->PMcrossover(parents);
    mutation_scramble(children);
    this->nextGeneration(children);
    this->calcAllSizes();
  }
}

// Genetic::Genetic(std::string filename) {
//   this->path.reserve(198000);
//   double best_dist = INT_MAX, cur_dist = 0;
//   int start_num;
//   double start_x, start_y;
//   int num;
//   double x, y;
//   char comma;
//   int shift;
//   bool unchanged = false;
//   feasible_city fs;
//   std::ifstream fin(filename);
//   fin.seekg(10, std::ios::beg);
//   fin >> start_num >> comma >> start_x >> comma >> start_y;
//   while (!unchanged) {
//     unchanged = true;
//     fin.clear();
//     fin.seekg(10, std::ios::beg);
//     this->path.push_back(start_num);
//     while (fin >> num >> comma >> x >> comma >> y) {
//       // cur_dist = dist(start_x, start_y, x, y);
//       if (cur_dist < best_dist && cur_dist && !this->visited(num)) {
//         unchanged = false;
//         best_dist = cur_dist;
//         fs.number = num;
//         fs.x = x;
//         fs.y = y;
//       }
//     }
//     this->path_size += best_dist;
//     start_num = fs.number;
//     start_x = fs.x;
//     start_y = fs.y;
//     best_dist = INT_MAX;
//     cur_dist = 0;
//   }
//   fin.close();
// }



Genetic::Genetic(std::string filename) {
  this->filename = filename;
  std::ifstream fin(this->filename);
  this->coord.reserve(1001);
  double best_dist = INT_MAX, cur_dist = 0;
  int start_num;
  double start_x, start_y;
  int num;
  double x, y;
  char comma;
  fin.seekg(10, std::ios::beg);
  while (fin >> num >> comma >> x >> comma >> y && this->coord.size() < 1000) {
      XY cr{x, y};
      this->coord.push_back(cr);
  }
  fin.close();
}

void Genetic::setFile(std::string filename) {
  this->filename = filename;
}



ACO::ACO(std::string filename, int read_count, int byte_shift) {
  this->filename = filename;
  std::ifstream fin(this->filename);
  this->coord.reserve(read_count);
  double best_dist = INT_MAX, cur_dist = 0;
  int start_num;
  double start_x, start_y;
  int num;
  double x, y;
  char comma;
  fin.seekg(byte_shift, std::ios::beg);
  while (fin >> num >> comma >> x >> comma >> y && this->coord.size() < read_count) {
    XY cr{x, y};
    this->coord.push_back(cr);
  }
  fin.close();
  this->count = this->coord.size();
  this->visited.resize(this->count);
  std::fill(this->visited.begin(), this->visited.end(), 0);
  this->roads.resize(this->coord.size());
  for (size_t i = 0; i < this->coord.size(); i++) {
    this->roads[i].resize(this->coord.size());
    Road temp{1, 0, false};
    std::fill(this->roads[i].begin(), this->roads[i].end(), temp);
  }
}

void ACO::setFile(std::string filename) {
  this->filename = filename;
}

void ACO::calcRoadPheromone(int from, int to) {
  this->roads[from][to].pheromone = (evaporation_rate * this->roads[from][to].pheromone) + this->roads[from][to].delta_pheromone;
  this->roads[to][from].pheromone = this->roads[from][to].pheromone;
}

void ACO::calcFullPheromone(std::vector<int>& tour, double dist) {
  for (int i = 0; i < tour.size() - 1; ++i)
  {
    this->roads[tour[i]][tour[i+1]].delta_pheromone += Q / dist;
    this->roads[tour[i+1]][tour[i]].delta_pheromone = this->roads[tour[i]][tour[i+1]].delta_pheromone;
  }
  for (int i = 0; i < this->count; ++i)
  {
    for (int j = 0; j < this->count; ++j)
    {
      calcRoadPheromone(i, j);
    }
  }
}

double ACO::calcDistance(int from, int to) {
  if (from != to)
    return dist(this->coord[from], this->coord[to]);
  else
    return INT_MAX;
}

void ACO::antColonyOptimization(int iterations) {
  std::vector<int> tour;
  double dist;
  for (size_t i = 0; i < iterations; i++) {
    tour = this->findAntWay();
    dist = calcTourDist(tour);
    calcFullPheromone(tour, dist);
    std::fill(this->visited.begin(), this->visited.end(), false);
    if (dist < this->best_size) {
      this->best_size = dist;
      this->best_tour = tour;
      std::cout << this->best_size;
      std::cout << "\n";
      for (size_t i = 0; i < this->best_tour.size(); i++) {
        std::cout << this->best_tour[i] << ' ';
      }
      std::cout << "\n";
    } else
      std::cout << ".\n";
  }
}

int ACO::nextCity(int from) {
  std::vector<double> chances;
  chances.reserve(this->count);
  double sum_chances = 0;
  for (std::size_t to = 0; to < this->count; ++to) {
    if (!visited[to])
      // this->calcPheromone(from, to)
      chances.push_back(this->roads[from][to].pheromone * pow((1 / this->calcDistance(from, to)), beta));
    else
      chances.push_back(0);
    sum_chances += chances[to];
  }
  for (std::size_t i = 0; i < this->count; ++i) {
    chances[i] /= sum_chances;
    if (i > 0) {
      chances[i] += chances[i - 1];
    }
  }

  std::random_device rd;
  std::mt19937 mersenne(rd());
  std::uniform_real_distribution<double> distr(0,1);
  double value = distr(mersenne);
  std::size_t to;
  for (std::size_t i = 0; i < this->count; ++i) {
    if (value <= chances[i]) {
      to = i;
      break;
    }
  }
  visited[to] = true;
  this->roads[from][to].was = true;
  this->roads[to][from].was = true;
  return to;
  // [0.1, 0.6, 0.2, 0.1]
  // [0.1, 0.7, 0.9, 1]
}

std::vector<int> ACO::findAntWay(int size) {
 std::random_device rd;
 std::mt19937 mersenne(rd());
 std::vector<int> way;
 int start_city = mersenne() % size;
 int cur_city = start_city;
 way.push_back(start_city);
 visited[start_city] = true;
 for (size_t i = 0; i < size - 1; i++) {
    cur_city = nextCity(cur_city);
    way.push_back(cur_city);
 }
 this->roads[start_city][way.back()].was = true;
 this->roads[way.back()][start_city].was = true;
 way.push_back(start_city);
 return way;
}

double ACO::calcTourDist(std::vector<int>& tour) {
  double&& tour_size = 0;
  for (int i = 0; i < tour.size() - 1;i++) {// 0 - 999 path[i] 0 - 999 coord 0 - 999
    tour_size += dist(this->coord[tour[i]], this->coord[tour[i+1]]);
  }
  return tour_size;
}

