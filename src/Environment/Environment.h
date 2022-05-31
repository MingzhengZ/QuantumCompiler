//
// Created by mingz on 2022/4/20.
//

#ifndef QUANTUMCOMPER_ENVIRONMENT_H
#define QUANTUMCOMPER_ENVIRONMENT_H

#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>
# include <vector>
# include <string>
# include <set>
# include <map>

struct ParsedGate {
    char * type;
    int target;
    int control;
};


using namespace std;

class Environment {
public:
    Environment(vector<vector<int>> coupling_list,string qasm_filename);

    //chip information
    int chip_num;
    int qubitUsed;
    vector<vector<int>> coupling_graph_list;
    vector<vector<int>> coupling_graph_matrix;
    vector<vector<int>> MakeCouplingGraph(vector<vector<int>> couplingList);
    void PrintCoupling();

    //circuit information:
    char * qasm_version = NULL;//qasm version
    vector<char*> includes;//list of include statements we need to reproduce in output
    vector<char*> customGates;//list of gate definitions we need to reproduce in output
    vector<char*> opaqueGates;//list of opaque gate definitions we need to reproduce in output
    vector<std::pair<int, int> > measures;//list of measurement gates; first is qbit, second is cbit
    vector<char*> qregName;
    vector<int> qregSize;

    int getQregOffset(char * name);
    int getCregOffset(char * name);

    vector<ParsedGate> gate_info;
    vector<int> gate_id_topo;
    vector<vector<int>> DagTable;
    vector<vector<int>> generateDag(vector<int> gateID);//gateid����Ҫ�������
    vector<vector<int>> getNewKLayerDag(vector<int> excuted_gateid, int K);//excuted_gateids���Ѿ�ִ���˵��ŵ���Ŀ
    vector<vector<int>> getKLayerDag(vector<int> excuted_gateid, int K);//excuted_gateids���Ѿ�ִ���˵��ŵ���Ŀ

    //necessary info for mapping original cbit IDs to flat array (and back again, if necessary)
    vector<char*> cregName;
    vector<int> cregSize;

    int circuit_num;//circuit qubit num
    int cir_depth;//circuit depth
    int gate_num;//circuit gate num
    vector<ParsedGate> parse(const char * fileName);

    //dag depth;
    int dag_depth;

    //��оƬ������ص�����
    map<string,float> chip_error;


};


#endif //QUANTUMCOMPER_ENVIRONMENT_H
