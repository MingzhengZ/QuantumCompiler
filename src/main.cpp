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
    cout << "how many path has done: " << a.finalPath.size() << endl;

}


int main() {
    string qasm_filename="../circuits/small/alu-v3_35.qasm";
    string qasm_filename0="../circuits/small/3_17_13.qasm";
    string qasm_filename1="../circuits/large/cm82a_208.qasm";
    vector<vector<int>>  coupling_map_list_qx2 = {{0, 1}, {0, 2}, {1, 2}, {2, 3}, {2, 4}, {3, 4}};
    vector<vector<int>> coupling_map_list_mel={{0,1},{1,2},{1,13},{2,3},{2,12},{3,4},
                                               {3,11},{4,5},{4,10},{5,6},{5,9},{6,8},
                                               {7,8},{8,9},{9,10},{10,11},{11,12},{12,13}};
    vector<vector<int>> coupling_map_list_asp={{0,1},{0,7},{1,2},{1,14},{2,3},{2,13},{3,4},{4,5},{5,6},{6,7},{8,9},{8,15},{9,10},{10,11},{11,12},{12,13},{13,14},{14,15}};
    Environment *env = new Environment(coupling_map_list_mel,qasm_filename1);
    //env->PrintCoupling();
    vector<int> executed;
    vector<vector<int>> k_dag = env->generateDag(env->gate_id_topo);
    vector<int> mapping_5={0,1,2,3,4};
    vector<int> mapping_14={0,1,2,3,4,5,6,7,8,9,10,11,12,13};
//    vector<int> qubitState(env->circuit_num, 0);
//    vector<ActionPath> newPath;
//    vector<vector<int>> allDag = env->generateDag(env->gate_id_topo);
//    SearchNode *sn =new SearchNode(mapping,qubitState,allDag, env, 0, newPath);
//    Search *sr = new Search(env);
//    SearchResult a = sr->SearchCircuit(sn);
//    PrintPath(a);

    clock_t startTime,endTime;
    startTime=clock();
    Search *sr = new Search(env);

    //SearchResult a= sr->SearchFullCircuit(mapping);
    SearchResult a= sr->SearchSmoothWithInitialMapping(mapping_14,2);
    endTime=clock();
    PrintPath(a);
    cout<<"time is "<<(double)(endTime-startTime)/CLOCKS_PER_SEC<<endl;
    return 0;
}
