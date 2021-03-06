//
// Created by mingz on 2022/4/24.
//

#ifndef QUANTUMCOMPER_DEFAULTEXPANDER_H
#define QUANTUMCOMPER_DEFAULTEXPANDER_H

#include "../Queue/Queue.h"
#include "../Queue/DefaultQueue.hpp"
#include "SearchNode.h"
#include "../Queue/Filter.h"
#include "../Queue/HashFilter_TOQM.hpp"
#include "../Environment/Environment.h"
#include <algorithm>
# include <set>

class DefaultExpander{
private:
    Environment* env;
public:
    bool findBestNode;
    int expandeNum;
    int cycleNum;
    int debug=0;
    vector<ActionPath> actionPath;
    vector<int> initialMapping;
    static int nodeCount;
//    void DeleteVectorElement(vector<int> V,int element);
    DefaultExpander(Environment* env);

    bool IsMoreCnot(SearchNode* node);

    /*Given the ready gates, give their combination,
     * if not all of them are combined, then the pattern is encountered
     */
    vector<vector<int>> ComReadyGates(vector<int> readyGates);
    /*For nodes that do not need swap, execute the path to the end
     *Set findBestNode to true
     *Set the final path to actionPath
     *The number of expands in this step is 1
     */
    void expandWithoutSwap(SearchNode* node);
    /* Input idle logic bits,
     * Output a combination of swaps that can be executed
     */
    vector<vector<vector<int>>> SwapCom(vector<int> qubitState,vector<int> l2pmapping);
    vector<vector<vector<int>>> SwapCom1(vector<int> qubitState,vector<int> l2pmapping,int qubit_num,set<int> cnot_qubit);
    vector<vector<vector<int>>> SwapCom2(vector<int> qubitState,vector<int> l2pmapping,int qubit_num,set<int> cnot_qubit);


    bool ExpandWithoutCnotCheck( DefaultQueue* nodes,SearchNode* node,HashFilter_TOQM* filter_T);
    bool ExpandWithCnotCheck( DefaultQueue* nodes,SearchNode* node,HashFilter_TOQM* filter_T);
    bool ExpandWithoutCnotCheckTopN(DefaultQueue *nodes, SearchNode *node, HashFilter_TOQM* filter_T ,int topN);
    bool IsCycle(vector<ActionPath> actionPath,int qubitNum);
};



#endif //QUANTUMCOMPER_DEFAULTEXPANDER_H
