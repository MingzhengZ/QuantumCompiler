//
// Created by mingz on 2022/4/22.
//

#include "SearchNode.h"


#include "SearchNode.h"

SearchNode::SearchNode(vector<int> nowMapping, vector<int> qubitState, vector<int> unscheduled_gateids, Environment *env,
                       int nowtime, vector<ActionPath> path) {
    this->l2pMapping = nowMapping;
    this->dead = false;
    this->qubitNum = nowMapping.size();
    this->p2lMapping.resize(this->qubitNum);
    this->dagTable = env->generateDag(unscheduled_gateids);
    this->gate2Critiality();
    this->environment = env;
    this->logicalQubitState = qubitState;
    this->timeStamp = nowtime;
    this->p2lMapping.resize(this->qubitNum);
    for (int i = 0; i < qubitNum; i++) {
        this->p2lMapping[this->l2pMapping[i]] = i;
    }
    this->computeCost1();
    this->remainGate=unscheduled_gateids;
    this->actionPath = path;
}

SearchNode::SearchNode(vector<int> nowMapping, vector<int> qubitState, vector<vector<int>> dag_table, Environment *env,
                       int nowtime, vector<ActionPath> path) {
    this->l2pMapping = nowMapping;
    this->dead = false;
    this->qubitNum = nowMapping.size();
    this->p2lMapping.resize(this->qubitNum);
    this->dagTable = dag_table;
    this->gate2Critiality();
    this->environment = env;
    this->logicalQubitState = qubitState;
    this->timeStamp = nowtime;
    this->p2lMapping.resize(this->qubitNum);
    for (int i = 0; i < qubitNum; i++) {
        this->p2lMapping[this->l2pMapping[i]] = i;
    }
    this->computeCost1();
    findRemainGates();
    this->actionPath = path;
}

void SearchNode::findRemainGates() {
    int depth = this->dagTable[0].size();
    for (int i = 0; i < depth; i++) {
        set<int> gatesId;
        for (int j = 0; j < this->dagTable.size(); j++) {
            if (this->dagTable[j][i] != -1 && gatesId.find(this->dagTable[j][i]) == gatesId.end()) {
                gatesId.insert(this->dagTable[j][i]);
                this->remainGate.push_back(this->dagTable[j][i]);
            }
        }
    }
}

int SearchNode::getReadyGateSize(){
    vector<int> newReadyGate;
    set<int> frontLayerGate;//判断是在第一层，没有依赖关系
    for(int j=0;j<this->dagTable.size();j++){
        if(this->dagTable[j][0]!=-1){
            frontLayerGate.insert(this->dagTable[j][0]);
        }
    }
    //判断比特空闲,如果在上一个set里，并且空闲，那么就可以加入到newReadyGate
    for (set<int>::iterator iter = frontLayerGate.begin(); iter != frontLayerGate.end(); ++iter){
        ParsedGate nowGate=this->environment->gate_info[*iter];
        int ii = this->p2lMapping[nowGate.target];
        int jj = this->p2lMapping[nowGate.control];
        if (nowGate.control == -1) {
            if (this->logicalQubitState[nowGate.target] == 0) {
                newReadyGate.push_back(*iter);
            }
        }
        //two qubits gate,两比特门还要多一个判断，是不是相邻的
        else if (this->logicalQubitState[nowGate.target] == 0 && this->logicalQubitState[nowGate.control] == 0 &&
                 this->environment->coupling_graph_matrix[ii][jj] == 1) {
            newReadyGate.push_back(*iter);
        } else {
        }
    }
    return newReadyGate.size();
}

void SearchNode::gate2Critiality() {
    int depth = this->dagTable[0].size();
    for (int i = 0; i < depth; i++) {
        for (int j = 0; j < this->dagTable.size(); j++) {
            int gateid = dagTable[j][i];
            if(gateid!=-1){
                if (this->gateCriticality.find(gateid) == this->gateCriticality.end()) {
                    int criticality = depth - i-1;
                    this->gateCriticality.insert({gateid, criticality});
                }
            }
        }
    }
}

int SearchNode::findFreeTime(int qubit, int gateID) {
    int path = 0;
    int i=0;
    while (dagTable[qubit][i] != gateID) {
        if(dagTable[qubit][i]!=-1){
            path++;
        }
        i++;
    }
    path = path + this->logicalQubitState[qubit];
    return path;
}

int SearchNode::busyTime() {
    int waitTime = 0;
    for (int i = 0; i < this->qubitNum; i++) {
        if (waitTime < this->logicalQubitState[i]) {
            waitTime = this->logicalQubitState[i];
        }
    }
    return waitTime;
}

void SearchNode::computeCost1() {
    int waitTime = busyTime();
    int cost = waitTime;
    vector<int> frontLayer = this->findFrontTwoQubitsGates();
    int gateNum = frontLayer.size();
    int costT = 99999;
    unordered_map<int, int> gateCost;
    for (int i = 0; i < gateNum; i++) {
        int gateId = frontLayer[i];
//        cout<<"gateId : "<<gateId<<endl;
        int qubit1 = this->environment->gate_info[gateId].control;
        int qubit2 = this->environment->gate_info[gateId].target;
//        int length1 = this->findFreeTimePhysical(this->p2lMapping[qubit1]);
        int length1 = this->findFreeTime(qubit1, gateId);
//        cout << "logical qubit is " << qubit1 << " and the length is " << length1 <<endl;
//        int length2 = this->findFreeTimePhysical(this->p2lMapping[qubit2]);
        int length2 = this->findFreeTime(qubit2, gateId);
//        cout << "logical qubit is " << qubit2 << " and the length is " << length2 << endl;
        if (length1 < length2) {
            std::swap(length1, length2);
        }
        int minSwaps=this->environment->coupling_graph_matrix[this->p2lMapping[qubit1]][this->p2lMapping[qubit2]]-1;
        if(minSwaps < costT) costT = minSwaps;
        int totalSwap = this->environment->coupling_graph_matrix[this->p2lMapping[qubit1]][this->p2lMapping[qubit2]]-1;
//        cout<<this->p2lMapping[qubit1]<<" "<<this->p2lMapping[qubit2]<<endl;
//        cout<<"total Swap is: "<<totalSwap<<endl;
        int totalSwapCost = totalSwap * 3;
        int slack = length1 - length2;
        int effectiveSlack = (slack / 3) * 3;
        if (effectiveSlack > totalSwapCost) {
            effectiveSlack = totalSwapCost;
        }
        int mutualSwapCost = totalSwapCost - effectiveSlack;
        int extraSwapCost = (0x1 & (mutualSwapCost / 3)) * 3;
        mutualSwapCost -= extraSwapCost;
        mutualSwapCost = mutualSwapCost >> 1;
        int gate_criticality=this->gateCriticality.find(gateId)->second;
        int cost1 = 1+gate_criticality + length1 + mutualSwapCost;
        int cost2 = 1+gate_criticality + length2 + mutualSwapCost + effectiveSlack;
//        cout<<"cost1 is "<<cost1<<" "<<"cost2 is "<<cost2<<" critial is "<<this->gateCriticality.find(gateId)->second<<" mutualSwapCost is : "<< mutualSwapCost<<" effectiveSlack is : "<<effectiveSlack<<endl;

        if (cost1 < cost2) {
            cost1 += extraSwapCost;
        } else {
            cost2 += extraSwapCost;
        }
        if (cost1 > cost) {
            cost = cost1;
        }
        if (cost2 > cost) {
            cost = cost2;
        }
    }
    if(costT == 99999) costT = 0;
    this->cost2 = costT;
    this->cost1 = cost + this->timeStamp;
}

vector<int> SearchNode::findFrontTwoQubitsGates() {
    vector<int> frontTwoQubitsGates;
    map<int, int> gateCount;
    int depth = this->dagTable[0].size();
    for (int i = 0; i < this->qubitNum; i++) {
        for (int j = 0; j < depth; j++) {
            if (this->dagTable[i][j] == -1) {
                continue;
            } else {
                if (this->environment->gate_info[this->dagTable[i][j]].control != -1) {
                    if (gateCount.find(this->dagTable[i][j]) == gateCount.end()) {
                        gateCount.insert({this->dagTable[i][j], 1});
                    } else {
                        gateCount.find(this->dagTable[i][j])->second++;
                    }
                    break;
                }
            }
        }
    }
    map<int, int>::iterator miter;
    miter = gateCount.begin();
    while (miter != gateCount.end()) {
        if (miter->second == 2) {
            frontTwoQubitsGates.push_back(miter->first);
        }
        miter++;
    }
    return frontTwoQubitsGates;
}

void SearchNode::PrintNode() {
    cout<<"---------------------------------------------"<<endl;
    cout<<"node time "<<this->timeStamp<<endl;
    cout<<"the mapping now is : ";
    for(int i=0;i<this->l2pMapping.size();i++){
        cout<<this->l2pMapping[i]<<" ";
    }
    cout<<endl;
    cout<<"the p2l mapping now is : ";
    for(int i=0;i<this->p2lMapping.size();i++){
        cout<<this->p2lMapping[i]<<" ";
    }
    cout<<endl;
    cout<<"the cost1 is : "<<this->cost1<<endl;
    cout<<"the qubits state are :";
    for(int i=0;i<this->qubitNum;i++){
        cout<<this->logicalQubitState[i]<<" ";
    }
    cout<<endl;
    cout<<"the remain gates are:";
    for(int i=0;i<this->remainGate.size();i++){
        cout<<remainGate[i]<<" ";
    }
    cout<<endl;
    cout<<"the action path is : \n";
    for(int i=0;i<this->actionPath.size();i++){
        //cout<<"the "<<i<<" step, the step length is"<<actionPath[i].actions.size()<<endl;
        for(int j=0;j<actionPath[i].actions.size();j++){
            cout<<actionPath[i].actions[j].gateID<<" "<<actionPath[i].actions[j].gateName<<"     ";
        }
        cout<<endl;
    }
    cout<<endl<<endl;
}
