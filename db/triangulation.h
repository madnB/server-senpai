#pragma once

#include <vector>
#include <map>
#include <algorithm>
#include <iterator>
#include "schema/helperclass.h"
#include "schema/schema_original.h"
#include "schema/schema_triang.h"

using namespace std;

class Triangulation {
  static map<string, Point> roots;
  static vector<Distance> distances;
public:
  //Triangulation(map<std::string, Point>);
  static void initTriang(map<string, Point> vector_dati);
  static Point triangolate(vector<schema_original>, int);
  static float rssi2meter(int rssi);
  static vector<Point> findPoints(Distance, map<string, float>);
  static Point findTruePoint(vector<Point>);
  static map<string, Point> getRoots() { return roots; }
  static void setRoots(map<string,Point> r) { roots = r; }
  static vector<Distance> getDistances() { return distances; }
  static void setDistances(vector<Distance> d) { distances = d; }
};
