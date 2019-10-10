#include "triangulation.h"
#include <limits>
#include <set>
#include <iostream>
#include <functional>
#include "schema/helperclass.h"

using namespace std;

map<string, Point> Triangulation::roots;
vector<Distance> Triangulation::distances;

float distance(Point point1, Point point2){
  return sqrt(pow(point1.x - point2.x, 2) +pow(point1.y - point2.y, 2) * 1.0);
}

void kLargest(Column* arr, int k) {

	typedef function<bool(pair<int, float>, pair<int, float>)> Comparator;
	Comparator compFunctor = 	[](std::pair<int, float> elem1 ,std::pair<int, float> elem2) { return elem1.second <= elem2.second; };
 
	set<pair<int, float>, Comparator> setOfWords(arr->distances.begin(), arr->distances.end(), compFunctor);

	for (int i = 0; i < k; i++)
	  arr->topk.push_back(next(setOfWords.begin(),i)->first);
}

void Triangulation::initTriang(map<string, Point> r) {
  Triangulation::setRoots(r);
  auto distances = Triangulation::getDistances();

  for(map<string, Point>::iterator it=r.begin(); it!=r.end(); ++it) {
    for(map<string, Point>::iterator it2=next<>(it, 1); it2!=r.end(); ++it2){
      Point point1=it->second;
      Point point2=it2->second;
      float distance=sqrt(pow(point1.x - point2.x, 2) +pow(point1.y - point2.y, 2) * 1.0);
      Distance dist=Distance(distance, it->first, it2->first);
      distances.push_back(dist);
    }
  }
  
  for(vector<Distance>::iterator it=distances.begin(); it!=distances.end(); it++)
    cout<<"Distance between "<<it->mac1<<" and "<<it->mac2<<": "<<it->distance<<endl;
  cout<<endl;
}

Point Triangulation::triangolate(vector<schema_original> vector_dati, int N_schede) {
  auto distances = Triangulation::getDistances();
  map<string, float> dists;
  vector<Point> allpoints;

  for(auto v : vector_dati)
    dists.insert(pair<string, float>(v.root, Triangulation::rssi2meter(v.RSSI)));
  
  for(vector<Distance>::iterator distIt=distances.begin(); distIt!=distances.end(); distIt++){
      vector<Point> points=Triangulation::findPoints(*distIt, dists);

      for(vector<Point>::iterator pointsIt=points.begin(); pointsIt!=points.end(); pointsIt++)
	      allpoints.push_back(*pointsIt);
    }
    
    Point result=Triangulation::findTruePoint(allpoints);
    cout<<"Point found: x="<<result.x<<" y="<<result.y<<endl;
    cout<<endl;
    return result;
}

float Triangulation::rssi2meter(int rssi){
  return rssi / 1.0;
}

vector<Point> Triangulation::findPoints(Distance rootDistance, map<string, float> dists) {
  auto roots = Triangulation::getRoots();
  vector<Point> ret;
  float r0=dists.find(rootDistance.mac1)->second;
  float x0=roots.find(rootDistance.mac1)->second.x;
  float y0=roots.find(rootDistance.mac1)->second.y;
  float r1=dists.find(rootDistance.mac2)->second;
  float x1=roots.find(rootDistance.mac2)->second.x;
  float y1=roots.find(rootDistance.mac2)->second.y;
  float d=rootDistance.distance;
  if(d>(r0+r1)||d<(sqrt(pow(r0-r1, 2))))
    return ret;
  float a=(pow(r0,2)-pow(r1,2)+pow(d,2))/(2*d);
  float h=sqrt(pow(r0,2)-pow(a,2));
  float x2=x0+a*(x1-x0)/d;
  float y2=y0+a*(y1-y0)/d;
  float x31=x2+h*(y1-y0)/d;
  float x32=x2-h*(y1-y0)/d;
  float y31=y2-h*(x1-x0)/d;
  float y32=y2+h*(x1-x0)/d;
  Point point1=Point(x31, y31);
  Point point2=Point(x32, y32);
  ret.push_back(point1);
  ret.push_back(point2);
  return ret;
}

Point Triangulation::findTruePoint(vector< Point > points) {
  auto roots = Triangulation::getRoots();

  vector<Column> matrix;
  int k=roots.size()*(roots.size()-1)/2;
  
  for(int i=0; i<points.size(); i++){
    matrix.push_back(Column(i));
    
    for(int j=0; j<points.size(); j++){
      if(i==j) matrix[i].distances.insert(pair<int, float>(j, numeric_limits<float>::max()));
      else matrix[i].distances.insert(pair<int, float>(j, distance(points[i], points[j])));
    }
    	
    kLargest(&matrix[i],k);
  }
  
  map<int, int> reps;
  for(int i=0; i<points.size(); i++)
    reps.insert(pair<int,int>(i,0));
  
  for(int i=0; i<points.size(); i++){
    for(int j=0; j<k; j++){
      reps[matrix[i].topk[j]]++;
    }
  }

  typedef function<bool(pair<int, int>, pair<int, int>)> Comparator;
  Comparator compFunctor = [](pair<int, int> elem1 ,std::pair<int, int> elem2) { return elem1.second >= elem2.second; };

  set<pair<int, int>, Comparator> setOfWords(reps.begin(), reps.end(), compFunctor);
  
  vector<int> topkpos;
  for(int i=0; i<k; i++)
    topkpos.push_back(next(setOfWords.begin(),i)->first);
  
  vector<Point> meanPoints;
  for(int i=0; i<topkpos.size(); i++)
    meanPoints.push_back(points[topkpos[i]]);
    
  float sumx=0;
  float sumy=0;
  
  for(auto m : meanPoints){
    sumx+=m.x;
    sumy+=m.y;
  }
  
  float x=sumx/meanPoints.size();
  float y=sumy/meanPoints.size();
  Point retval=Point(x,y);

  return retval;
}


