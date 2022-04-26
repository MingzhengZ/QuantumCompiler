//
// Created by mingz on 2022/4/23.
//

#ifndef STRONGESTMAPPER_QUEUE1_HPP
#define STRONGESTMAPPER_QUEUE1_HPP
#include "Queue.h"

#include <queue>
#include <vector>
#include <iostream>
#include "../Search/SearchNode.h"
#include "HashFilter_TOQM.hpp"

extern bool _verbose;

//Queue example: this queue uses std::priority_queue
class Queue1 : public Queue {
private:
    struct CmpDefaultQueue
    {
        bool operator()(const SearchNode* lhs, const SearchNode* rhs) const
        {
            //tiebreaker:
            if(lhs->cost1 == rhs->cost1) {
                //return lhs->scheduled->size > rhs->scheduled->size;
                //return lhs->numUnscheduledGates > rhs->numUnscheduledGates;
                //return lhs->cycle < rhs->cycle;
            }

            //lower cost is better
        }
    };

    std::priority_queue<SearchNode*, std::vector<SearchNode*>, CmpDefaultQueue> nodes;

    bool pushNode(SearchNode * newNode) {
        //std::cerr << "Debug message: pushed node with cost " << newNode->cost << "\n";
        nodes.push(newNode);
        return true;
    }


public:
    SearchNode * pop() {
        numPopped++;

        SearchNode * ret = nodes.top();
        nodes.pop();
        return ret;
    }

    int size() {
        return nodes.size();
    }
};

#endif //STRONGESTMAPPER_QUEUE1_HPP
