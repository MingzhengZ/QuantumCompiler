#include <iostream>
#include <time.h>
#include "Environment/Environment.h"
#include "Search/SearchNode.h"
#include "Search/Search.h"

void PrintPath(SearchResult a) {
    cout << "how many path has done: " << a.finalPath.size() << endl;
    for (int i = 0; i < a.finalPath.size(); i++) {
        for (int j = 0; j < a.finalPath[i].actions.size(); j++) {
            cout << a.finalPath[i].actions[j].gateID << " " << a.finalPath[i].actions[j].gateName << " "
                 << a.finalPath[i].actions[j].controlQubit << " " << a.finalPath[i].actions[j].targetQubit << "   ";
        }
        cout << endl;
    }
    int count = 0;
    for (int i = 0; i < a.searchNodeNum.size(); i++) {
        count = count + a.searchNodeNum[i];
    }
    cout << "search node number: " << count << endl;
    int pattern = 0;
    for (int i = 0; i < a.finalPath.size(); i++) {
        if (a.finalPath[i].pattern == true) {
            pattern++;
        }
    }
    cout << "pattern number: " << pattern << endl;
    cout << "cycle num is " << a.cycleNum << endl;

}


int main() {
    string qasm_filename="D:\\study\\github\\sMapper\\circuits\\small\\alu-v3_34.qasm";
    vector<vector<int>>  coupling_map_list_qx2 = {{0, 1}, {0, 2}, {1, 2}, {2, 3}, {2, 4}, {3, 4}};
    Environment *env = new Environment(coupling_map_list_qx2,qasm_filename);
    vector<int> mapping={0,1,2,3,4};
//    vector<int> qubitState(env->circuit_num, 0);
//    vector<ActionPath> newPath;
//    vector<vector<int>> allDag = env->generateDag(env->gate_id_topo);
//    SearchNode *sn =new SearchNode(mapping,qubitState,allDag, env, 0, newPath);
//    Search *sr = new Search(env);
//    SearchResult a = sr->SearchCircuit(sn);
//    PrintPath(a);


//    DefaultQueue *nodeQueue = new DefaultQueue();
//    HashFilter_TOQM * filterT =new HashFilter_TOQM();
//    nodeQueue->push(sn);
//    DefaultExpander nodeExpander(env);
//    bool ifFind;
//    SearchNode *expandeNode;
//    expandeNode = nodeQueue->pop();
//    ifFind=nodeExpander.ExpandWithoutCnotCheck(nodeQueue, expandeNode,filterT);
//    cout<<"find it? "<<ifFind<<endl;
//    cout<<nodeQueue->size();

    clock_t startTime,endTime;
    startTime=clock();
    Search *sr = new Search(env);

    SearchResult a= sr->SearchFullCircuit(mapping);
    endTime=clock();
    PrintPath(a);
    cout<<"time is "<<(endTime-startTime)/CLOCKS_PER_SEC<<endl;
    return 0;
}
