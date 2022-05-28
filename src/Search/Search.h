//
// Created by mingz on 2022/4/26.
//

#ifndef QUANTUMCOMPER_SEARCH_H
#define QUANTUMCOMPER_SEARCH_H

#include "../Search/SearchNode.h"
#include "../Environment/Environment.h"
#include "DefaultExpander.h"
#include "../Queue/DefaultQueue.hpp"
#include "../Queue/Queue1.hpp"
#include "../Queue/HashFilter_TOQM.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <math.h>

using namespace std;

struct SearchResult{
    vector<ActionPath> finalPath;
    vector<int> initialMapping;
    vector<int> finalMapping;
    vector<int> searchNodeNum;
    vector<int> queueNum;
    int patternNum;
    int cycleNum;
    int swapNum;
};

class Search {
private:
    int debug=0;
public:
    Environment* env;
    Search(Environment* env);

    SearchResult SearchCircuit(SearchNode* sn);

    SearchResult SearchCircuitCnotGreedy(SearchNode* sn);

    SearchResult SearchCircuitGreedy(SearchNode *sn,int topNnum);

    //搜索完这个线路的所有节点
    SearchResult SearchFullCircuit(vector<int> mapping);
    //smooth search
    SearchResult SearchSmoothWithInitialMapping(vector<int> mapping,int k);
    //smooth search with topN greedy
    SearchResult SearchSmoothWithInitialMappingAndGreedy(vector<int> mapping,int k,int topN);
    //smooth search adapt
    SearchResult SearchSmoothWithInitialMappingAdpat(vector<int> mapping,int searchNodeNum);
    //smooth more layer
    SearchResult SearchSmoothWithInitialMappingMoreLayer(vector<int> mapping,int k,int moreLayer);
    //smooth more layer with adpat
    SearchResult SearchSmoothWithInitialMappingAdpatMoreLayer(vector<int> mapping,int searchNodeNum);

};


#endif //QUANTUMCOMPER_SEARCH_H
