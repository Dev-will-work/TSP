#include "TSP.h"
#include <ctime>
#include <cmath>
#include <string>
#include <fstream>
#include <iostream>

double dist(double x1, double y1, double x2, double y2) {
  double dist = sqrt( (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2) );
  return dist;
}

bool Graph::visited(int number) {
  for (size_t i = 0; i < this->path.size(); i++) {
    if (path[i] == number) return true;
  }
  return false;
}

double Graph::calcRouteSize() {

}

Graph::Graph(std::string filename) {
  this->path.reserve(198000);
  double best_dist = INT_MAX, cur_dist = 0;
  int start_num;
  double start_x, start_y;
  int num;
  double x, y;
  char comma;
  int shift;
  bool unchanged = false;
  feasible_city fs;
  std::ifstream fin(filename);
  fin.seekg(10, std::ios::beg);
  fin >> start_num >> comma >> start_x >> comma >> start_y;
  while (!unchanged) {
    unchanged = true;
    fin.clear();
    fin.seekg(10, std::ios::beg);
    this->path.push_back(start_num);
    while (fin >> num >> comma >> x >> comma >> y) {
      cur_dist = dist(start_x, start_y, x, y);
      if (cur_dist < best_dist && cur_dist && !this->visited(num)) {
        unchanged = false;
        best_dist = cur_dist;
        fs.number = num;
        fs.x = x;
        fs.y = y;
      }
    }
    this->path_size += best_dist;
    start_num = fs.number;
    start_x = fs.x;
    start_y = fs.y;
    best_dist = INT_MAX;
    cur_dist = 0;
  }
  fin.close();
}

Graph::Graph() {
  std::ifstream fin(this->filename);
  this->coord.reserve(198000);
  double best_dist = INT_MAX, cur_dist = 0;
  int start_num;
  double start_x, start_y;
  int num;
  double x, y;
  char comma;
  int shift;
  fin.close();
}

Graph::~Graph() {

}