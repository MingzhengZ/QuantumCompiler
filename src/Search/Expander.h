//
// Created by mingz on 2022/4/24.
//

#ifndef QUANTUMCOMPER_EXPANDER_H
#define QUANTUMCOMPER_EXPANDER_H

#include "../Queue/Queue.h"
#include "../Queue/DefaultQueue.hpp"
#include "SearchNode.h"
#include "../Queue/Filter.h"
#include "../Queue/HashFilter_TOQM.hpp"
#include "../Environment/Environment.h"
#include <algorithm>

class Expander {
public:
    //best node answer
    bool findBestNode;
    int expandeNum;
    int cycleNum;
    vector<ActionPath> actionPath;
    vector<int> initialMapping;

    virtual ~Expander() {};

    //expands given node, unless it has same-or-worse cost than best final node
    //returns false iff given node's cost >= best final node's cost
    virtual bool expand(DefaultQueue *nodes,SearchNode* node) =0;

};

#endif //QUANTUMCOMPER_EXPANDER_H
