#ifndef TSP_H
#define TSP_H
#include <vector>
#include <fstream>
double dist(double x1, double y1, double x2, double y2);
int min(std::vector<double>, std::vector<bool>);

struct feasible_city {
  double x;
  double y;
  int number;
};

struct Coord {
  double x;
  double y;
};

class Graph {
 public:
  Graph();
  Graph(std::string);
  ~Graph();
  bool visited(int number);

  void add_vertex(std::string, int, int);
void greedy(int);
  bool all_visited();
  double calcGain(int, int, int, int);
  void best_2_opt();
  void first_2_opt();
  void calcRouteSize();
   void setPathSize(double size);
   double getPathSize();
   int getVertexes();
   void setVertexes(int vertexes);
   void setPathList(std::vector<int>);
   std::vector<int> getPathList();
   bool valid_edge(std::vector<std::string> solution, std::string from, std::string to);
   void perturbation();
   void swap(int i, int j);
void twoOpt(int i, int j, int k, int l);

 private:
  std::string filename;
  double path_size = 0;
  std::vector<int> path;
  std::vector<Coord> coord;
};
#endif
