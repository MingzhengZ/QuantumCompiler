//
// Created by mingz on 2022/4/22.
//

#ifndef QUANTUMCOMPER_SEARCHNODE_H
#define QUANTUMCOMPER_SEARCHNODE_H

#include <vector>
#include <string>
#include <unordered_map>
#include "../Environment/Environment.h"
#include <map>

using namespace std;

struct ScheduledGate{
    string gateName;
    int targetQubit;
    int controlQubit;
    int gateID;
};

struct ActionPath{
    vector<ScheduledGate> actions;
    bool pattern;
};

class SearchNode {
private:
    unordered_map<int,int> gateCriticality;
    void computeCost1();
    void gate2Critiality();
    int findFreeTime(int physicalQubit,int gateID);
    int busyTime();
    //这个的主要目的是为了判断pattern的数目

public:
    Environment * environment;
    vector<int> findFrontTwoQubitsGates();
    bool dead;
    int timeStamp;
    int qubitNum;
    //initail mapping
    vector<int> initialMapping;
    //logical on physical
    vector<int> l2pMapping;
    //physical on logical
    vector<int> p2lMapping;
    //logical qubit state busy time
    vector<int> logicalQubitState;
    //dagTable
    vector<vector<int>> dagTable;


    //heuristic value and timestamp
    int cost1;
    int cost2;
    //unscheduled gate
    vector<int> remainGate;
    void findRemainGates();
    //action path
    vector<ActionPath> actionPath;

    //待执行的门是用vector的形式输入
    SearchNode(vector<int>nowMapping,vector<int>qubitState,vector<int> unscheduled_gateids,Environment *env,int nowtime,vector<ActionPath> path);
    //待执行的门用dagTable的形式输入
    SearchNode(vector<int>nowMapping,vector<int>qubitState,vector<vector<int>> dag_table,Environment *env,int nowtime,vector<ActionPath> path);

    //和错误相关的数据
    float error_rate;//执行到当前结点的正确率

    //内部使用的一些功能
    void PrintNode();
    int getReadyGateSize();
};




#endif //QUANTUMCOMPER_SEARCHNODE_H
