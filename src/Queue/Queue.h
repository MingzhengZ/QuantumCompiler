//
// Created by mingz on 2022/04/23.
//

#ifndef STRONGESTMAPPER_QUEUE_H
#define STRONGESTMAPPER_QUEUE_H
#include "../Search/SearchNode.h"
#include "../Environment/Environment.h"
#include "Filter.h"
#include <iostream>

class Queue {
private:
    //push a node into the priority queue
    //return false iff this fails for any reason
    //pre-condition: our filters have already said this node is good
    //pre-condition: newNode.cost has already been set
    virtual bool pushNode(SearchNode * newNode) = 0;

protected:
    SearchNode * bestFinalNode = 0;


public:
    int numPushed=0,numFiltered=0,numPopped=0;
    virtual ~Queue() {};

    virtual int setArgs(char** argv) {
        //used to set the queue's parameters via command-line
        //return number of args consumed

        return 0;
    }

    virtual int setArgs() {
        //used to set the queue's parameters via std::cin
        //return number of args consumed

        return 0;
    }

    //pop a node and return it
    virtual SearchNode* pop() = 0;

    //return number of elements in queue
    virtual int size() = 0;

    //push a node into the priority queue
    //return false iff this fails for any reason
    //pre-condition: newNode.cost has already been set
    virtual bool push(SearchNode *node) = 0;

    inline SearchNode * getBestFinalNode() {
        return bestFinalNode;
    }
};
#endif //STRONGESTMAPPER_QUEUE_H
