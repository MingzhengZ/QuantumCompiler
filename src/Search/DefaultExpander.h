//
// Created by mingz on 2022/4/24.
//

#ifndef QUANTUMCOMPER_DEFAULTEXPANDER_H
#define QUANTUMCOMPER_DEFAULTEXPANDER_H

# include "Expander.h"
# include <set>

class DefaultExpander: public Expander {
private:
    Environment* env;
public:
    static int nodeCount;
//    void DeleteVectorElement(vector<int> V,int element);
    DefaultExpander(Environment* env);

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

    bool ExpandWithoutCnotCheck( DefaultQueue* nodes,SearchNode* node,HashFilter_TOQM* filter_T);
    bool IsCycle(vector<ActionPath> actionPath,int qubitNum);
};



#endif //QUANTUMCOMPER_DEFAULTEXPANDER_H
