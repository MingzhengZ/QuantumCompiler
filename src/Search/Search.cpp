//
// Created by mingz on 2022/4/26.
//

#include "Search.h"

Search::Search(Environment *env) {
    this->env = env;
}

SearchResult Search::SearchCircuit(SearchNode *sn) {
    DefaultQueue *nodeQueue = new DefaultQueue();
    HashFilter_TOQM * filterT =new HashFilter_TOQM();
    nodeQueue->push(sn);
    DefaultExpander nodeExpander(this->env);
    vector<int> searchNum;
    int cycleNum=0;
    int whilecount=0;
    while (nodeQueue->size() >= 0) {
        whilecount++;
//        cout<<"while count = "<<whilecount<<"and the queue size is "<<nodeQueue->size()<<endl;
        bool ifFind;
        SearchNode *expandeNode;
        expandeNode = nodeQueue->pop();
//        expandeNode->PrintNode();
        ifFind=nodeExpander.ExpandWithoutCnotCheck(nodeQueue, expandeNode,filterT);
        searchNum.push_back(nodeExpander.expandeNum);
        cycleNum=cycleNum+nodeExpander.cycleNum;
        if (ifFind == true) {
            break;
        }
    }
    SearchResult sr;
    sr.finalPath = nodeExpander.actionPath;
    sr.searchNodeNum = searchNum;
    int count=0;
    for (int i = 0; i < sr.searchNodeNum.size(); i++) {
        count = count + sr.searchNodeNum[i];
    }
    cout << "search node number: " << count << endl;
    vector<int> queueNum;
    queueNum.push_back(nodeQueue->numPushed);
    sr.queueNum = queueNum;
    sr.cycleNum=cycleNum;
    return sr;
}

SearchResult Search::SearchCircuitCnotGreedy(SearchNode *sn) {
    DefaultQueue *nodeQueue = new DefaultQueue();
    HashFilter_TOQM * filterT =new HashFilter_TOQM();
    nodeQueue->push(sn);
    DefaultExpander nodeExpander(this->env);
    vector<int> searchNum;
    int cycleNum=0;
    int whilecount=0;
    while (nodeQueue->size() >= 0) {
        whilecount++;
//        cout<<"while count = "<<whilecount<<"and the queue size is "<<nodeQueue->size()<<endl;
        bool ifFind;
        SearchNode *expandeNode;
        expandeNode = nodeQueue->pop();
//        expandeNode->PrintNode();
        ifFind=nodeExpander.ExpandWithCnotCheck(nodeQueue, expandeNode,filterT);
        searchNum.push_back(nodeExpander.expandeNum);
        cycleNum=cycleNum+nodeExpander.cycleNum;
        if (ifFind == true) {
            break;
        }
    }
    SearchResult sr;
    sr.finalPath = nodeExpander.actionPath;
    sr.searchNodeNum = searchNum;
    int count=0;
    for (int i = 0; i < sr.searchNodeNum.size(); i++) {
        count = count + sr.searchNodeNum[i];
    }
    cout << "search node number: " << count << endl;
    vector<int> queueNum;
    queueNum.push_back(nodeQueue->numPushed);
    sr.queueNum = queueNum;
    sr.cycleNum=cycleNum;
    return sr;
}

SearchResult Search::SearchCircuitGreedy(SearchNode *sn,int topNnum) {
    DefaultQueue *nodeQueue = new DefaultQueue();
    HashFilter_TOQM * filterT =new HashFilter_TOQM();
    nodeQueue->push(sn);
    DefaultExpander nodeExpander(this->env);
    vector<int> searchNum;
    int cycleNum=0;
    int whilecount=1;
    while (nodeQueue->size() >= 0) {
        whilecount++;
        //cout<<"while count = "<<whilecount<<"and the queue size is "<<nodeQueue->size()<<endl;
        bool ifFind;
        SearchNode *expandeNode;
        expandeNode = nodeQueue->pop();
        //expandeNode->PrintNode();
        ifFind=nodeExpander.ExpandWithoutCnotCheckTopN(nodeQueue, expandeNode,filterT,topNnum);
        searchNum.push_back(nodeExpander.expandeNum);
        cycleNum=cycleNum+nodeExpander.cycleNum;
        if (ifFind == true) {
            break;
        }
    }
    SearchResult sr;
    sr.finalPath = nodeExpander.actionPath;
    sr.searchNodeNum = searchNum;
    int count;
    for (int i = 0; i < sr.searchNodeNum.size(); i++) {
        count = count + sr.searchNodeNum[i];
    }
    cout << "search node number: " << count << endl;
    vector<int> queueNum;
    queueNum.push_back(nodeQueue->numPushed);
    sr.queueNum = queueNum;
    sr.cycleNum=cycleNum;
    return sr;
}

SearchResult Search::SearchFullCircuit(vector<int> mapping) {
    vector<ActionPath> newPath;
    int qubitNum = this->env->circuit_num;
    vector<int> qubitState(qubitNum, 0);
    vector<vector<int>> allDag = this->env->generateDag(env->gate_id_topo);
    SearchNode *sn =new SearchNode(mapping,qubitState,allDag, env, 0, newPath);
    Search *sr = new Search(env);
    SearchResult a = sr->SearchCircuit(sn);
    return a;
}

SearchResult Search::SearchSmoothWithInitialMappingAndGreedy(vector<int> mapping, int k,int topN) {
    vector<int> originMapping = mapping;
    int qubitNum = this->env->circuit_num;
    vector<int> qubitState(qubitNum, 0);
    vector<vector<int>> allDag = this->env->generateDag(env->gate_id_topo);
    vector<int> topoGate=this->env->gate_id_topo;
    int nowTime=0;
    vector<ActionPath> path;
    if(allDag[0].size()<=k){
        //�������С��k�㣬��ô�Լ�������ͺ�
        SearchNode *sn = new SearchNode(mapping, qubitState, allDag, env, nowTime, path);
        Search *sr = new Search(env);
        delete sr;
        SearchResult a = this->SearchCircuitGreedy(sn,topN);
        return a;
    }
    else{
        //�����������k�㣬��ôÿ��ȡǰk���dag
        SearchResult searR;
        searR.cycleNum=0;
        searR.patternNum=0;
        searR.swapNum=0;
        searR.initialMapping=mapping;
        vector<int> nowMapping=mapping;
        vector<int> nowQubitState(qubitNum, 0);
        vector<ActionPath>finalPath;
        vector<int> executedgateIDs;
        while(topoGate.size()>0){
            vector<ActionPath> newPath;
//            cout<<"executedgateIDs: ";
//            for(int i=0;i<executedgateIDs.size();i++){
//                cout<<executedgateIDs[i]<<" ";
//            }
//            cout<<endl;
            vector<vector<int>> kDag=env->getNewKLayerDag(executedgateIDs,k);
            if(0){
                cout<<"========================================"<<endl;
                for(int i=0;i<kDag[0].size();i++){
                    for(int j=0;j<kDag.size();j++){
                        cout<<kDag[j][i]<<" ";
                    }
                    cout<<endl;
                }
                cout<<"the k-dag depth is "<<kDag[0].size()<<endl;
            }
            if(kDag[0].size()<k){
                //������µ�ֻ��k���ˣ���ô��ֱ��������
                SearchNode *sn = new SearchNode(nowMapping, nowQubitState, kDag, env, nowTime, newPath);
                cout<<"search node done"<<endl;
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuitGreedy(sn,topN);
                //������ÿ������ݷŵ�ԭ����final path�ͳ�Ƽ���swapNum����Ŀ
                for(int i=0;i<a.finalPath.size();i++){
                    finalPath.push_back(a.finalPath[i]);
                    for(int j=0;j<a.finalPath[i].actions.size();j++){
                        if(a.finalPath[i].actions[j].gateName=="swap"){
                            searR.swapNum++;
                        }
                    }
                }
                //�޸�searchResult�����ͳ������
                int patternNum=0;
                for(int i=0;i<finalPath.size();i++){
                    if(finalPath[i].pattern==true){
                        patternNum++;
                    }
                }
                searR.finalPath=finalPath;
                searR.patternNum=patternNum;
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                return searR;
            }
            else{
                //cout<<"k=5 begin search node"<<endl;
                SearchNode *sn =new SearchNode(nowMapping,nowQubitState,kDag, env, nowTime, newPath);
//                cout<<"search node done"<<endl;
                //sn->PrintNode();
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuitGreedy(sn,topN);
                //ȡ���һ���Ľ��״̬
                finalPath.push_back(a.finalPath[0]);
                int swapNum;
                for(int i=0;i<a.finalPath[0].actions.size();i++){
                    if (a.finalPath[0].actions[i].gateName == "swap") {
                        //�����swap�޸�mappingӳ��������Լ�qubitState
                        int phyQubit1 = a.finalPath[0].actions[i].targetQubit;
                        int phyQubit2 = a.finalPath[0].actions[i].controlQubit;
                        int temp = nowMapping[phyQubit1];
                        nowMapping[phyQubit1] = nowMapping[phyQubit2];
                        nowMapping[phyQubit2] = temp;
                        nowQubitState[nowMapping[phyQubit1]] = 3;
                        nowQubitState[nowMapping[phyQubit2]] = 3;
                        swapNum++;
                    }
                    else {
                        //�����ִ�еĽ�㣬topoGateɾ��������
                        executedgateIDs.push_back(a.finalPath[0].actions[i].gateID);
                    }
                }
                //�޸�qubitState
                for(int i=0;i<nowQubitState.size();i++){
                    if(nowQubitState[i]>0){
                        nowQubitState[i]--;
                    }
                }
                nowTime++;
                //����searchResult���������
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                searR.swapNum=searR.swapNum+swapNum;
                delete sr;
            }
        }
        return searR;
    }
}

SearchResult Search::SearchSmoothWithInitialMapping(vector<int> mapping, int k) {
    vector<int> originMapping = mapping;
    int qubitNum = this->env->circuit_num;
    cout<<"qubit num "<<this->env->circuit_num<<endl;
    vector<int> qubitState(qubitNum, 0);
    vector<vector<int>> allDag = this->env->generateDag(env->gate_id_topo);
    vector<int> topoGate=this->env->gate_id_topo;
    int nowTime=0;
    vector<ActionPath> path;
    if(allDag[0].size()<=k){
        //�������С��k�㣬��ô�Լ�������ͺ�
        SearchNode *sn = new SearchNode(mapping, qubitState, allDag, env, nowTime, path);
        Search *sr = new Search(env);
        delete sr;
        SearchResult a = this->SearchCircuit(sn);
        return a;
    }
    else{
        //�����������k�㣬��ôÿ��ȡǰk���dag
        SearchResult searR;
        searR.cycleNum=0;
        searR.patternNum=0;
        searR.swapNum=0;
        searR.initialMapping=mapping;
        vector<int> nowMapping=mapping;
        vector<int> nowQubitState(qubitNum, 0);
        vector<ActionPath>finalPath;
        vector<int> executedgateIDs;
        while(topoGate.size()>0){
            vector<ActionPath> newPath;
//            cout<<"executedgateIDs: ";
//            for(int i=0;i<executedgateIDs.size();i++){
//                cout<<executedgateIDs[i]<<" ";
//            }
//            cout<<endl;
            vector<vector<int>> kDag=env->getNewKLayerDag(executedgateIDs,k);
            if(0){
                cout<<"========================================"<<endl;
                for(int i=0;i<kDag[0].size();i++){
                    for(int j=0;j<kDag.size();j++){
                        cout<<kDag[j][i]<<" ";
                    }
                    cout<<endl;
                }
                cout<<"the k-dag depth is "<<kDag[0].size()<<endl;
            }
            if(kDag[0].size()<k){
                //������µ�ֻ��k���ˣ���ô��ֱ��������
                SearchNode *sn = new SearchNode(nowMapping, nowQubitState, kDag, env, nowTime, newPath);
                //cout<<"search node done"<<endl;
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuit(sn);
                //������ÿ������ݷŵ�ԭ����final path�ͳ�Ƽ���swapNum����Ŀ
                for(int i=0;i<a.finalPath.size();i++){
                    finalPath.push_back(a.finalPath[i]);
                    for(int j=0;j<a.finalPath[i].actions.size();j++){
                        if(a.finalPath[i].actions[j].gateName=="swap"){
                            searR.swapNum++;
                        }
                    }
                }
                //�޸�searchResult�����ͳ������
                int patternNum=0;
                for(int i=0;i<finalPath.size();i++){
                    if(finalPath[i].pattern==true){
                        patternNum++;
                    }
                }
                searR.finalPath=finalPath;
                searR.patternNum=patternNum;
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                return searR;
            }
            else{
                //cout<<"k=5 begin search node"<<endl;
                SearchNode *sn =new SearchNode(nowMapping,nowQubitState,kDag, env, nowTime, newPath);
//                cout<<"search node done"<<endl;
                //sn->PrintNode();
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuit(sn);
                //ȡ���һ���Ľ��״̬
                finalPath.push_back(a.finalPath[0]);
                if(0){
                    cout<<"search one layer "<<endl;
                    for(int i=0;i<a.finalPath.size();i++){
                        for(int j=0;j<a.finalPath[i].actions.size();j++){
                            cout << a.finalPath[i].actions[j].gateID << " " << a.finalPath[i].actions[j].gateName << " "
                                 << a.finalPath[i].actions[j].controlQubit << " " << a.finalPath[i].actions[j].targetQubit << "   ";
                        }
                        cout<<endl;
                    }
                }

                int swapNum;
                for(int i=0;i<a.finalPath[0].actions.size();i++){
                    if (a.finalPath[0].actions[i].gateName == "swap") {
                        //�����swap�޸�mappingӳ��������Լ�qubitState
                        int phyQubit1 = a.finalPath[0].actions[i].targetQubit;
                        int phyQubit2 = a.finalPath[0].actions[i].controlQubit;
                        int temp = nowMapping[phyQubit1];
                        nowMapping[phyQubit1] = nowMapping[phyQubit2];
                        nowMapping[phyQubit2] = temp;
                        nowQubitState[nowMapping[phyQubit1]] = 3;
                        nowQubitState[nowMapping[phyQubit2]] = 3;
                        swapNum++;
                    }
                    else {
                        //�����ִ�еĽ�㣬topoGateɾ��������
                        executedgateIDs.push_back(a.finalPath[0].actions[i].gateID);
                    }
                }
                //�޸�qubitState
                for(int i=0;i<nowQubitState.size();i++){
                    if(nowQubitState[i]>0){
                        nowQubitState[i]--;
                    }
                }
                nowTime++;
                //����searchResult���������
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                searR.swapNum=searR.swapNum+swapNum;

                delete sr;
            }
        }
        return searR;
    }
}

SearchResult Search::SearchSmoothWithInitialMappingAdpat(vector<int> mapping, int searchNodeNum) {
    int k=3;
    vector<int> originMapping = mapping;
    int qubitNum = this->env->circuit_num;
    vector<int> qubitState(qubitNum, 0);
    vector<vector<int>> allDag = this->env->generateDag(env->gate_id_topo);
    vector<int> topoGate=this->env->gate_id_topo;
    int nowTime=0;
    vector<ActionPath> path;
    if(allDag[0].size()<=k){
        //�������С��k�㣬��ô�Լ�������ͺ�
        SearchNode *sn = new SearchNode(mapping, qubitState, allDag, env, nowTime, path);
        Search *sr = new Search(env);
        SearchResult a = this->SearchCircuitCnotGreedy(sn);
        return a;
    }
    else{
        //�����������k�㣬��ôÿ��ȡǰk���dag
        SearchResult searR;
        searR.cycleNum=0;
        searR.patternNum=0;
        searR.swapNum=0;
        searR.initialMapping=mapping;
        vector<int> nowMapping=mapping;
        vector<int> nowQubitState(qubitNum, 0);
        vector<ActionPath>finalPath;
        vector<int> executedgateIDs;
        while(topoGate.size()>0){
            vector<ActionPath> newPath;
            vector<vector<int>> kDag=env->getNewKLayerDag(executedgateIDs,k);
            cout<<"the k-dag depth is "<<kDag[0].size()<<endl;
            for(int i=0;i<kDag[0].size();i++){
                for(int j=0;j<kDag.size();j++){
                    cout<<kDag[j][i]<<" ";
                }
                cout<<endl;
            }
            if(kDag[0].size()<k){
                //������µ�ֻ��k���ˣ���ô��ֱ��������
                SearchNode *sn = new SearchNode(nowMapping, nowQubitState, kDag, env, nowTime, newPath);
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuitCnotGreedy(sn);
                //������ÿ������ݷŵ�ԭ����final path�ͳ�Ƽ���swapNum����Ŀ
                for(int i=0;i<a.finalPath.size();i++){
                    finalPath.push_back(a.finalPath[i]);
                    for(int j=0;j<a.finalPath[i].actions.size();j++){
                        if(a.finalPath[i].actions[j].gateName=="swap"){
                            searR.swapNum++;
                        }
                    }
                }
                //�޸�searchResult�����ͳ������
                int patternNum=0;
                for(int i=0;i<finalPath.size();i++){
                    if(finalPath[i].pattern==true){
                        patternNum++;
                    }
                }
                searR.finalPath=finalPath;
                searR.patternNum=patternNum;
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                return searR;
            }
            else{
                cout<<"a new layer "<<endl;
                SearchNode *sn = new SearchNode(nowMapping, nowQubitState, kDag, env, nowTime, newPath);
                //sn->PrintNode();
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuitCnotGreedy(sn);
                //ȡ���һ���Ľ��״̬
                finalPath.push_back(a.finalPath[0]);
                int swapNum;
                for(int i=0;i<a.finalPath[0].actions.size();i++){
                    if (a.finalPath[0].actions[i].gateName == "swap") {
                        //�����swap�޸�mappingӳ��������Լ�qubitState
                        int phyQubit1 = a.finalPath[0].actions[i].targetQubit;
                        int phyQubit2 = a.finalPath[0].actions[i].controlQubit;
                        int temp = nowMapping[phyQubit1];
                        nowMapping[phyQubit1] = nowMapping[phyQubit2];
                        nowMapping[phyQubit2] = temp;
                        nowQubitState[nowMapping[phyQubit1]] = 3;
                        nowQubitState[nowMapping[phyQubit2]] = 3;
                        swapNum++;
                    }
                    else {
                        //�����ִ�еĽ�㣬topoGateɾ��������
                        executedgateIDs.push_back(a.finalPath[0].actions[i].gateID);
                    }
                }
                //�޸�qubitState
                for(int i=0;i<nowQubitState.size();i++){
                    if(nowQubitState[i]>0){
                        nowQubitState[i]--;
                    }
                }
                nowTime++;
                //����searchResult���������
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                //����Ӧ���޸���������
                if(searNum>searchNodeNum){
                    k--;
                }
                else{
                    k++;
                }
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                searR.swapNum=searR.swapNum+swapNum;
            }
        }
        return searR;
    }
}

SearchResult Search::SearchSmoothWithInitialMappingMoreLayer(vector<int> mapping,int k,int moreLayer){
    vector<int> originMapping = mapping;
    int qubitNum = this->env->circuit_num;
    vector<int> qubitState(qubitNum, 0);
    vector<vector<int>> allDag = this->env->generateDag(env->gate_id_topo);
    vector<int> topoGate=this->env->gate_id_topo;
    int nowTime=0;
    vector<ActionPath> path;
    if(allDag[0].size()<=k){
        //�������С��k�㣬��ô�Լ�������ͺ�
        SearchNode *sn = new SearchNode(mapping, qubitState, allDag, env, nowTime, path);
        Search *sr = new Search(env);
        SearchResult a = this->SearchCircuit(sn);
        return a;
    }
    else{
        //�����������k�㣬��ôÿ��ȡǰk���dag
        SearchResult searR;
        searR.cycleNum=0;
        searR.patternNum=0;
        searR.swapNum=0;
        searR.initialMapping=mapping;
        vector<int> nowMapping=mapping;
        vector<int> nowQubitState(qubitNum, 0);
        vector<ActionPath>finalPath;
        vector<int> executedgateIDs;
        while(topoGate.size()>0){
            vector<ActionPath> newPath;
            vector<vector<int>> kDag=env->getNewKLayerDag(executedgateIDs,k);
            cout<<"the k-dag depth is "<<kDag[0].size()<<endl;
            if(kDag[0].size()<k){
                //������µ�ֻ��k���ˣ���ô��ֱ��������
                SearchNode *sn = new SearchNode(nowMapping, nowQubitState, kDag, env, nowTime, newPath);
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuit(sn);
                //������ÿ������ݷŵ�ԭ����final path�ͳ�Ƽ���swapNum����Ŀ
                for(int i=0;i<a.finalPath.size();i++){
                    finalPath.push_back(a.finalPath[i]);
                    for(int j=0;j<a.finalPath[i].actions.size();j++){
                        if(a.finalPath[i].actions[j].gateName=="swap"){
                            searR.swapNum++;
                        }
                    }
                }
                //�޸�searchResult�����ͳ������
                int patternNum=0;
                for(int i=0;i<finalPath.size();i++){
                    if(finalPath[i].pattern==true){
                        patternNum++;
                    }
                }
                searR.finalPath=finalPath;
                searR.patternNum=patternNum;
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                return searR;
            }
            else{
                cout<<"a new layer "<<endl;
                SearchNode *sn =new SearchNode(nowMapping,nowQubitState,kDag, env, nowTime, newPath);
                sn->PrintNode();
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuit(sn);
                //ȡ���һ���Ľ��״̬
                int swapNum;
                for(int nowLayer=0;nowLayer<moreLayer;nowLayer++){
                    finalPath.push_back(a.finalPath[nowLayer]);
                    for(int i=0;i<a.finalPath[nowLayer].actions.size();i++){
                        if (a.finalPath[nowLayer].actions[i].gateName == "swap") {
                            //�����swap�޸�mappingӳ��������Լ�qubitState
                            int phyQubit1 = a.finalPath[nowLayer].actions[i].targetQubit;
                            int phyQubit2 = a.finalPath[nowLayer].actions[i].controlQubit;
                            int temp = nowMapping[phyQubit1];
                            nowMapping[phyQubit1] = nowMapping[phyQubit2];
                            nowMapping[phyQubit2] = temp;
                            nowQubitState[nowMapping[phyQubit1]] = 3;
                            nowQubitState[nowMapping[phyQubit2]] = 3;
                            swapNum++;
                        }
                        else {
                            //�����ִ�еĽ�㣬topoGateɾ��������
                            executedgateIDs.push_back(a.finalPath[nowLayer].actions[i].gateID);
                        }
                    }
                    //�޸�qubitState
                    for(int i=0;i<nowQubitState.size();i++){
                        if(nowQubitState[i]>0){
                            nowQubitState[i]--;
                        }
                    }
                    nowTime++;
                }
                //����searchResult���������
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                searR.swapNum=searR.swapNum+swapNum;
            }
        }
        return searR;
    }
}

SearchResult Search::SearchSmoothWithInitialMappingAdpatMoreLayer(vector<int> mapping,int searchNodeNum){
    int k=5;
    vector<int> originMapping = mapping;
    int qubitNum = this->env->circuit_num;
    vector<int> qubitState(qubitNum, 0);
    vector<vector<int>> allDag = this->env->generateDag(env->gate_id_topo);
    vector<int> topoGate=this->env->gate_id_topo;
    int nowTime=0;
    vector<ActionPath> path;
    if(allDag[0].size()<=k){
        //�������С��k�㣬��ô�Լ�������ͺ�
        SearchNode *sn = new SearchNode(mapping, qubitState, allDag, env, nowTime, path);
        Search *sr = new Search(env);
        SearchResult a = this->SearchCircuit(sn);
        return a;
    }
    else{
        //�����������k�㣬��ôÿ��ȡǰk���dag
        SearchResult searR;
        searR.cycleNum=0;
        searR.patternNum=0;
        searR.swapNum=0;
        searR.initialMapping=mapping;
        vector<int> nowMapping=mapping;
        vector<int> nowQubitState(qubitNum, 0);
        vector<ActionPath>finalPath;
        vector<int> executedgateIDs;
        while(topoGate.size()>0){
            vector<ActionPath> newPath;
            vector<vector<int>> kDag=env->getNewKLayerDag(executedgateIDs,k);
            cout<<"the k-dag depth is "<<kDag[0].size()<<endl;
            if(kDag[0].size()<k){
                //������µ�ֻ��k���ˣ���ô��ֱ��������
                SearchNode *sn = new SearchNode(nowMapping, nowQubitState, kDag, env, nowTime, newPath);
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuit(sn);
                //������ÿ������ݷŵ�ԭ����final path�ͳ�Ƽ���swapNum����Ŀ
                for(int i=0;i<a.finalPath.size();i++){
                    finalPath.push_back(a.finalPath[i]);
                    for(int j=0;j<a.finalPath[i].actions.size();j++){
                        if(a.finalPath[i].actions[j].gateName=="swap"){
                            searR.swapNum++;
                        }
                    }
                }
                //�޸�searchResult�����ͳ������
                int patternNum=0;
                for(int i=0;i<finalPath.size();i++){
                    if(finalPath[i].pattern==true){
                        patternNum++;
                    }
                }
                searR.finalPath=finalPath;
                searR.patternNum=patternNum;
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                return searR;
            }
            else{
                cout<<"a new layer "<<endl;
                SearchNode *sn =new SearchNode(nowMapping,nowQubitState,kDag, env, nowTime, newPath);
                sn->PrintNode();
                Search *sr = new Search(env);
                SearchResult a = sr->SearchCircuit(sn);
                //����searchResult���������
                int searNum=0;
                for(int i=0;i<a.searchNodeNum.size();i++){
                    searNum=searNum+a.searchNodeNum[i];
                }
                searR.searchNodeNum.push_back(searNum);
                int queueNum=0;
                for(int i=0;i<a.queueNum.size();i++){
                    queueNum=queueNum+a.queueNum[i];
                }
                searR.queueNum.push_back(queueNum);
                searR.cycleNum=searR.cycleNum+a.cycleNum;
                //�ж����������ı仯,ȷ��ȷ�ϵĲ���
                int moreLayer;
                if(searNum>searchNodeNum){
                    k--;
                    moreLayer=ceil(searchNodeNum/searNum);
                }
                else{
                    k++;
                    moreLayer=1;
                }
                //ȡ���һ���Ľ��״̬
                int swapNum;
                for(int nowLayer=0;nowLayer<moreLayer;nowLayer++){
                    finalPath.push_back(a.finalPath[nowLayer]);
                    for(int i=0;i<a.finalPath[nowLayer].actions.size();i++){
                        if (a.finalPath[nowLayer].actions[i].gateName == "swap") {
                            //�����swap�޸�mappingӳ��������Լ�qubitState
                            int phyQubit1 = a.finalPath[nowLayer].actions[i].targetQubit;
                            int phyQubit2 = a.finalPath[nowLayer].actions[i].controlQubit;
                            int temp = nowMapping[phyQubit1];
                            nowMapping[phyQubit1] = nowMapping[phyQubit2];
                            nowMapping[phyQubit2] = temp;
                            nowQubitState[nowMapping[phyQubit1]] = 3;
                            nowQubitState[nowMapping[phyQubit2]] = 3;
                            swapNum++;
                        }
                        else {
                            //�����ִ�еĽ�㣬topoGateɾ��������
                            executedgateIDs.push_back(a.finalPath[nowLayer].actions[i].gateID);
                        }
                    }
                    //�޸�qubitState
                    for(int i=0;i<nowQubitState.size();i++){
                        if(nowQubitState[i]>0){
                            nowQubitState[i]--;
                        }
                    }
                    nowTime++;
                }
                searR.swapNum=searR.swapNum+swapNum;
            }
        }
        return searR;
    }
}
