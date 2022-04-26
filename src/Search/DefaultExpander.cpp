//
// Created by mingz on 2022/4/24.
//

#include "DefaultExpander.h"



DefaultExpander::DefaultExpander(Environment *env) {
    this->env=env;
    this->cycleNum=0;
    this->expandeNum=0;
}


/* Given the ready gates, give their combination,
 * if not all of them are combined, then the pattern is encountered
 */
vector<vector<int>> DefaultExpander::ComReadyGates(vector<int> readyGates) {
    vector<vector<int>> readyGateCom;
    vector<int> temp;
    readyGateCom.push_back(temp);
    for(int i=0;i<readyGates.size();i++){
        int now_size=readyGateCom.size();
        for(int j=0;j<now_size;j++){
            vector<int> temp1=readyGateCom[j];
            temp1.push_back(readyGates[i]);
            readyGateCom.push_back(temp1);
        }

    }
    return readyGateCom;
}

/* Input idle logic bits,
 * Output a combination of swaps that can be executed
 */
vector<vector<vector<int>>> DefaultExpander::SwapCom(vector<int> qubitState,vector<int> l2pmapping) {
    vector<vector<int>> possibleSwap;
    for(int i=0;i<this->env->coupling_graph_list.size();i++){
        int a=this->env->coupling_graph_list[i][0];
        int b=this->env->coupling_graph_list[i][1];
        if(qubitState[l2pmapping[a]]==0&&qubitState[l2pmapping[b]]==0){
            possibleSwap.push_back(env->coupling_graph_list[i]);
        }
    }
    vector<vector<vector<int>>> possibleSwapCom;
    vector<vector<int>> temp;
    possibleSwapCom.push_back(temp);
    for(int k=0;k<possibleSwap.size();k++) {
        int nowSwapSize = possibleSwapCom.size();
        for (int a = 0; a < nowSwapSize; a++) {
            set<int> usedQubits;
            for(int b=0;b<possibleSwapCom[a].size();b++){
                usedQubits.insert(possibleSwapCom[a][b][0]);
                usedQubits.insert(possibleSwapCom[a][b][1]);
            }
            if(usedQubits.count(possibleSwap[k][0])==0&&usedQubits.count(possibleSwap[k][1])==0){
                vector<vector<int>> temp1;
                temp1 = possibleSwapCom[a];
                temp1.push_back(possibleSwap[k]);
                possibleSwapCom.push_back(temp1);
            }
        }
    }
    return possibleSwapCom;
}

//Given action path to determine whether there is a loop
bool DefaultExpander::IsCycle(vector<ActionPath> actionPath, int qubitNum) {
    //如果isCycle是truen那么有循环，需要被筛除
    bool isCycle=false;
    int actionLongth=actionPath.size();
    //如果路径小于等于3那么不可能有循环，因为一个swap占据了3个时间周期
    if(actionLongth<=3){
        return isCycle;
    }
    //qubitSwap用于记录每个逻辑比特上执行的swap
    //qubitSwap[1]上面记录物理qubit[1]上和哪个物理比特swap了
    vector<int> qubitSwap(qubitNum,-2);
    ActionPath lastAction=actionPath[actionLongth-1];
    for(int i=0;i<lastAction.actions.size();i++){
        if(lastAction.actions[i].gateName=="swap") {
            isCycle = true;
            qubitSwap[lastAction.actions[i].controlQubit]=lastAction.actions[i].targetQubit;
            qubitSwap[lastAction.actions[i].targetQubit]=lastAction.actions[i].controlQubit;
        }
    }
    //当isCycle = true的时候，说明新增加的一轮里面没有swap,那么不可能造成新的cycle
    if(isCycle==false){
        return isCycle;
    }
    else{
        for(int i=actionLongth-2;i>=0;i--){
            for(int j=0;j<actionPath[i].actions.size();j++){
                if(actionPath[i].actions[j].gateName=="swap"){
                    int swap1=actionPath[i].actions[j].controlQubit;
                    int swap2=actionPath[i].actions[j].targetQubit;
                    if(qubitSwap[swap1]==swap2){
                        return true;
                    }
                    else{
                        if(qubitSwap[swap1]!=-2){
                            qubitSwap[qubitSwap[swap1]]=-2;
                            qubitSwap[swap1]=-2;
                        }
                        if(qubitSwap[swap2]!=-2){
                            qubitSwap[qubitSwap[swap2]]=-2;
                            qubitSwap[swap2]=-2;
                        }
                    }
                }
                if(actionPath[i].actions[j].gateName=="cx"){
                    int cx1=actionPath[i].actions[j].controlQubit;
                    int cx2=actionPath[i].actions[j].targetQubit;
                    if(qubitSwap[cx1]!=-2){
                        qubitSwap[qubitSwap[cx1]]=-2;
                        qubitSwap[cx1]=-2;
                    }
                    if(qubitSwap[cx2]!=-2){
                        qubitSwap[qubitSwap[cx2]]=-2;
                        qubitSwap[cx2]=-2;
                    }
                }
            }
            isCycle=false;
            for(int k=0;k<qubitNum;k++){
                if(qubitSwap[k]!=-2){
                    isCycle=true;
                }
            }
            if(isCycle==false){
                return isCycle;
            }
        }
        return false;
    }
}


bool DefaultExpander::ExpandWithoutCnotCheck(DefaultQueue *nodes, SearchNode *node, HashFilter_TOQM* filter_T) {
    int countNum=0;
    int cycleNum=0;
    //dead or not 如果这个结点已经死了，那么意味着队列里有比这个结点好的结点都使用过了
    if(node->dead==true){
        this->expandeNum=this->expandeNum+countNum;
        return false;
    }
    else{
        //先执行所有的swap组合，然后再是能执行的ready gate都执行
        vector<vector<vector<int>>> possibleSwap=this->SwapCom(node->logicalQubitState,node->l2pMapping);

        for(int i=0; i < possibleSwap.size(); i++){
            //执行所有的swap组合
            vector<int> qubitState1=node->logicalQubitState;
            vector<int> mapping=node->l2pMapping;
            vector<ScheduledGate> thisTimeSchduledGate;
            for(int j=0;j<possibleSwap[i].size(); j++){
                int a=possibleSwap[i][j][0];
                int b=possibleSwap[i][j][1];
                int temp;
                temp=mapping[a];
                mapping[a]=mapping[b];
                mapping[b]=temp;
                qubitState1[mapping[a]]=3;
                qubitState1[mapping[b]]=3;
                ScheduledGate Sg;
                Sg.targetQubit=possibleSwap[i][j][0];
                Sg.controlQubit=possibleSwap[i][j][1];
                Sg.gateName="swap";
                Sg.gateID=-1;
                thisTimeSchduledGate.push_back(Sg);
            }
            //计算p2lmapping 每个逻辑比特上的物理比特
            vector<int> p2lmapping(mapping.size(),-1);
            for(int j=0;j<mapping.size();j++){
                p2lmapping[mapping[j]]=j;
            }
            //执行所有能执行的门，依赖关系是在第一层没有前驱结点，比特要空闲，双比特门要相邻
            vector<int> newReadyGate;
            set<int> frontLayerGate;//判断是在第一层，没有依赖关系
            for(int j=0;j<node->dagTable.size();j++){
                if(node->dagTable[j][0]!=0){
                    frontLayerGate.insert(node->dagTable[j][0]);
                }
            }
            //判断比特空闲,如果在上一个set里，并且空闲，那么就可以加入到newReadyGate
            for (set<int>::iterator iter = frontLayerGate.begin(); iter != frontLayerGate.end(); ++iter){
                ParsedGate nowGate=this->env->gate_info[*iter];
                int ii = p2lmapping[nowGate.target];
                int jj = p2lmapping[nowGate.control];
                if (nowGate.control == -1) {
                    if (qubitState1[nowGate.target] == 0) {
                        newReadyGate.push_back(*iter);
                    }
                }
                //two qubits gate
                else if (qubitState1[nowGate.target] == 0 && qubitState1[nowGate.control] == 0 &&
                         this->env->coupling_graph_list[ii][jj] == 1) {
                    newReadyGate.push_back(*iter);
                } else {
                }
            }

            for(int j=0;j<newReadyGate.size();j++){
                //Add the actions performed by this step
                ScheduledGate Sg;
                Sg.targetQubit=node->p2lMapping[this->env->gate_info[newReadyGate[j]].target];
                if(this->env->gate_info[newReadyGate[j]].control=-1){
                    Sg.controlQubit=node->p2lMapping[this->env->gate_info[newReadyGate[j]].control];
                }
                else{
                    Sg.controlQubit=-1;
                }
                Sg.gateName=this->env->gate_info[newReadyGate[j]].type;
                Sg.gateID=newReadyGate[j];
                thisTimeSchduledGate.push_back(Sg);
                //update qubit state
                qubitState1[this->env->gate_info[newReadyGate[j]].target]=1;
                if(this->env->gate_info[newReadyGate[j]].control!=-1){
                    qubitState1[this->env->gate_info[newReadyGate[j]].control]=1;
                }
            }
            //执行完ready gate后，当前结点的状态
            vector<int> remainGate;
            for(int j=0;j<node->remainGate.size();j++){
                bool flag=true;
                for(int k=0;k<newReadyGate.size();k++){
                    if(newReadyGate[k]==node->remainGate[j]){
                        flag=false;
                        break;
                    }
                }
                if(flag==true){
                    remainGate.push_back(node->remainGate[j]);
                }
            }
            if(remainGate.size()==0){
                ActionPath thisAction;
                thisAction.actions=thisTimeSchduledGate;
                thisAction.pattern=true;
                vector<ActionPath> path=node->actionPath;
                path.push_back(thisAction);
                this->actionPath=path;
                this->expandeNum=countNum;
                return true;
            }
            //生成新的ActionPath
            ActionPath thisAction;
            thisAction.actions=thisTimeSchduledGate;
            bool IsPattern;
            if(node->getReadyGateSize()==newReadyGate.size()){
                IsPattern=false;
            }
            else{
                IsPattern=true;
            }
            thisAction.pattern=IsPattern;
            vector<ActionPath> path=node->actionPath;
            path.push_back(thisAction);
            //qubit state update and time
            for(int k=0;k<qubitState1.size();k++){
                if(qubitState1[k]>0){
                    qubitState1[k]--;
                }
            }
            int timeStamp=node->timeStamp+1;
            if(this->IsCycle(path,qubitState1.size())==false){
                countNum++;
                SearchNode* sn= new SearchNode(mapping,qubitState1,remainGate,env,timeStamp,path);
                //sn->PrintNode();
                if(!filter_T->filter(sn)){
                    //cout<<"accept"<<endl;
                    nodeCount++;
                    sn->nodeID=nodeCount;
                    nodes->push(sn);
                }
            }
            else{
                cycleNum++;
            }
        }
    }
    this->expandeNum=countNum;
    this->cycleNum=cycleNum;
    return false;
}

