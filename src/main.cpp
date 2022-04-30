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
    vector<vector<int>>  coupling_map_list_T = {{0, 1}, };
    vector<vector<int>> coupling_map_list_mel={{0,1},{1,2},{1,13},{2,3},{2,12},{3,4},
                                               {3,11},{4,5},{4,10},{5,6},{5,9},{6,8},
                                               {7,8},{8,9},{9,10},{10,11},{11,12},{12,13}};
    vector<vector<int>> coupling_map_list_asp={{0,1},{0,7},{1,2},{1,14},{2,3},{2,13},{3,4},{4,5},{5,6},{6,7},{8,9},{8,15},{9,10},{10,11},{11,12},{12,13},{13,14},{14,15}};
    Environment *env = new Environment(coupling_map_list_mel,qasm_filename1);
    //env->PrintCoupling();
    vector<int> executed;
    executed={0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,8 ,11 ,10 ,9 ,12 ,13 ,16 ,15 ,18 ,19 ,14 ,17 ,20 ,21 ,22 ,23 ,27 ,24 ,25 ,26 ,28 ,33 ,29 ,31 ,30 ,32 ,34 ,35 ,36 ,37
            ,38 ,39 ,40 ,43 ,41 ,42 ,44 ,45 ,47 ,46 ,48 ,49 ,50 ,51 ,52 ,53 ,55 ,54 ,59 ,56 ,58 ,57 ,60 ,61 ,63 ,64 ,62 ,67 ,65 ,66 ,68 ,69 ,70 ,71 ,72 ,75 ,74 ,73 ,76 ,77
            ,84 ,81 ,82 ,83 ,78 ,79 ,80 ,85 ,86 ,87 ,89 ,88 ,90 ,93 ,91 ,92 ,94 ,95 ,97 ,100 ,96 ,98 ,99};
    vector<vector<int>> k_dag = env->getNewKLayerDag(executed,2);
    vector<int> mapping_5={0,1,2,3,4};
    vector<int> mapping_14={12, 4 ,3 ,13 ,5, 10, 1, 8, 2 ,9 ,7 ,6 ,0 ,11};
    vector<int> qubitState(env->chip_num, 0);
    vector<ActionPath> newPath;
    vector<vector<int>> allDag = env->generateDag(env->gate_id_topo);
    clock_t startTime,endTime;
    startTime=clock();
    Search *sr = new Search(env);
    SearchResult a = sr->SearchSmoothWithInitialMappingAndGreedy(mapping_14,3,10);
    endTime=clock();
    PrintPath(a);
    cout<<"time is "<<(double)(endTime-startTime)/CLOCKS_PER_SEC<<endl;

//    clock_t startTime,endTime;
//    startTime=clock();
//    Search *sr = new Search(env);
//
//    //SearchResult a= sr->SearchFullCircuit(mapping);
//    SearchResult a= sr->SearchSmoothWithInitialMapping(mapping_14,2);
//    endTime=clock();
//    PrintPath(a);
//    cout<<"time is "<<(double)(endTime-startTime)/CLOCKS_PER_SEC<<endl;
    return 0;
}
