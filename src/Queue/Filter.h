//
// Created by mingz on 2021/12/27.
//

#ifndef STRONGESTMAPPER_FILTER_H
#define STRONGESTMAPPER_FILTER_H
#include "../Search/SearchNode.h"
#include <iostream>

class Filter{
public:
    virtual ~Filter() {};

    //this should be called after we're done scheduling gates in newNode
    //return true iff we don't want to add newNode to the nodes list
    virtual bool filter(SearchNode * newNode) = 0;

    virtual void printStatistics(std::ostream & stream) {
        //this function should print info such as how many nodes have been filtered out
    }

    virtual void deleteRecord(SearchNode * n) {
        //if this filter retains node info, delete the filter's records of node n
    }

//    virtual Filter * createEmptyCopy() = 0;

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
};



#endif //STRONGESTMAPPER_FILTER_H
