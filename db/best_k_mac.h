#pragma once
#include <set>
#include <vector>
using namespace std;

class best_k_mac {
public:
	vector<pair<string, vector<int>>> best_mac;
	vector<string> etichette_periodi;
    void insert(pair < string, vector<int>> mac_periods){
        best_mac.push_back(mac_periods);
    }

    best_k_mac(vector<string> etichette){
        vector<pair<string, vector<int>>> best_mac;
        etichette_periodi.assign(etichette.begin(),etichette.end());
    }
};
