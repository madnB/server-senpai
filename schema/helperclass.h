#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

class Column {
  public:
    int pos;

    Column();
    Column(int pos) { this->pos = pos; }

    map<int, float> distances;
    vector<int> topk;
};

class Distance {
   public:
    Distance(float distance, string mac1, string mac2)
    {
        this->distance = distance;
        this->mac1 = mac1;
        this->mac2 = mac2;
    }
    float distance;
    string mac2;
    string mac1;
};

class Point {
  public:
    float x;
    float y;
    Point(float x, float y) { this->x = x; this->y = y; }
};
