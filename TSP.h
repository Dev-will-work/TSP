#ifndef TSP_H
#define TSP_H
#include <vector>
#include <fstream>
#include <cstdlib>

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

class Graph {
 public:
  Graph();
  // // Graph(std::string);
  // ~Graph();
  // bool visited(int number);

  // void add_vertex(std::string, int, int);
  // void greedy(int);
  // bool all_visited();
  // double calcGain(int, int, int, int);
  // void best_2_opt();
  // void first_2_opt();
  // double calcPathSize();
  // void setPathSize(double size);
  // double getPathSize();
  // int getVertexes();
  // void setVertexes(int vertexes);
  // void setPathList(std::vector<int>);
  // std::vector<int> getPathList();
  // bool valid_edge(std::vector<std::string> solution, std::string from, std::string to);
  // void perturbation();
  // void swap(int i, int j);
  // void twoOpt(int i, int j, int k, int l);

 private:
  std::string filename;
  double path_size = 0;
  std::vector<int> path;
  std::vector<XY> coord;
};

#endif // TSP_H
