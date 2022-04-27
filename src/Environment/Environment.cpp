//
// Created by mingz on 2022/4/20.
//The part about qasm file reading is based on project a
//

#include "Environment.h"

///Gets next token in the OPENQASM file we're parsing
char * getToken(std::ifstream & infile, bool & sawSemicolon) {
    char c;
    int MAXBUFFERSIZE = 256;
    char buffer[MAXBUFFERSIZE];
    int bufferLoc = 0;
    bool paren = false;//true if inside parentheses, e.g. partway through reading U3(...) gate
    bool bracket = false;//true if inside brackets
    bool quote = false;//true if inside quotation marks
    bool comment = false;//true if between "//" and end-of-line

    if(sawSemicolon) {//saw semicolon in previous call, but had to return a different token
        sawSemicolon = false;
        char * token = new char[2];
        token[0]=';';
        token[1]=0;
        return token;
    }

    while(infile.get(c)) {
        assert(bufferLoc < MAXBUFFERSIZE);

        if(comment) {//currently parsing a single-line comment
            if(c == '\n') {
                comment = false;
            }
        } else if(quote) {
            buffer[bufferLoc++] = c;
            if(c == '"') {
                quote = false;
                buffer[bufferLoc++] = 0;
                char * token = new char[bufferLoc];
                strcpy(token, buffer);
                return token;
            }
        } else if(c == '"') {
            assert(!bufferLoc);
            quote = true;
            buffer[bufferLoc++] = c;
        } else if(c == '\r') {
        } else if(c == '/') {//probably parsing the start of a single-line comment
            if(bufferLoc && buffer[bufferLoc-1] == '/') {
                bufferLoc--;//remove '/' from buffer
                comment = true;
            } else {
                buffer[bufferLoc++] = c;
            }
        } else if(c == ';') {
            assert(!paren);
            assert(!bracket);

            if(bufferLoc == 0) {
                buffer[bufferLoc++] = c;
            } else {
                sawSemicolon = true;
            }

            buffer[bufferLoc++] = 0;
            char * token = new char[bufferLoc];
            strcpy(token, buffer);
            return token;
        } else if(c == ' ' || c == '\n' || c == '\t' || c == ',') {
            if(paren || bracket) {
                buffer[bufferLoc++] = c;
            } else if(bufferLoc) { //this whitespace is a token separator
                buffer[bufferLoc++] = 0;
                char * token = new char[bufferLoc];
                strcpy(token,buffer);
                return token;
            }
        } else if(c == '(') {
            assert(!paren);
            assert(!bracket);
            paren = true;
            buffer[bufferLoc++] = c;
        } else if(c == ')') {
            assert(paren);
            assert(!bracket);
            paren = false;
            buffer[bufferLoc++] = c;
        } else if(c == '[') {
            assert(!paren);
            assert(!bracket);
            bracket = true;
            buffer[bufferLoc++] = c;
        } else if(c == ']') {
            assert(!paren);
            assert(bracket);
            bracket = false;
            buffer[bufferLoc++] = c;
        } else {
            buffer[bufferLoc++] = c;
        }
    }

    if(bufferLoc) {
        buffer[bufferLoc++] = 0;
        char * token = new char[bufferLoc];
        strcpy(token,buffer);
        return token;
    } else {
        return 0;
    }
}
//returns entire gate definition (except the 'gate' keyword) as a string.
char * getCustomGate(std::ifstream & infile) {
    char c;
    int MAXBUFFERSIZE = 1024;
    char buffer[MAXBUFFERSIZE];
    int bufferLoc = 0;
    bool curlybrace = false;
    bool comment = false;//true if between "//" and end-of-line

    while(infile.get(c)) {
        assert(bufferLoc < MAXBUFFERSIZE);

        if(comment) {//currently parsing a single-line comment
            if(c == '\n') {
                comment = false;
            }
        } else if(c == '/') {//probably parsing the start of a single-line comment
            if(bufferLoc && buffer[bufferLoc-1] == '/') {
                bufferLoc--;//remove '/' from buffer
                comment = true;
            }
        } else if(c == '{') {
            assert(!curlybrace);
            curlybrace = true;
            buffer[bufferLoc++] = c;
        } else if(c == '}') {
            assert(curlybrace);
            buffer[bufferLoc++] = c;

            buffer[bufferLoc++] = 0;
            char * token = new char[bufferLoc];
            strcpy(token, buffer);
            return token;
        } else {
            buffer[bufferLoc++] = c;
        }
    }

    assert(false);
    return 0;
}
//returns the rest of the statement up to and including the semicolon at its end
//I use this for saving opaque gate statements
char * getRestOfStatement(std::ifstream & infile) {
    char c;
    int MAXBUFFERSIZE = 1024;
    char buffer[MAXBUFFERSIZE];
    int bufferLoc = 0;
    bool comment = false;//true if between "//" and end-of-line

    while(infile.get(c)) {
        assert(bufferLoc < MAXBUFFERSIZE);

        if(comment) {//currently parsing a single-line comment
            if(c == '\n') {
                comment = false;
            }
        } else if(c == '/') {//probably parsing the start of a single-line comment
            if(bufferLoc && buffer[bufferLoc-1] == '/') {
                bufferLoc--;//remove '/' from buffer
                comment = true;
            }
        } else if(c == ';') {
            buffer[bufferLoc++] = c;

            buffer[bufferLoc++] = 0;
            char * token = new char[bufferLoc];
            strcpy(token, buffer);
            return token;
        } else {
            buffer[bufferLoc++] = c;
        }
    }

    assert(false);
    return 0;
}

Environment::Environment(vector<vector<int>> coupling_list, string qasm_filename) {
    //假设这是一个全连通的芯片，确定芯片的比特数目
    set<int> phyQubits;
    for(int i=0;i<coupling_list.size();i++){
        phyQubits.insert(coupling_list[i][0]);
        phyQubits.insert(coupling_list[i][1]);
    }
    this->chip_num=phyQubits.size();
    //芯片的邻接表和邻接矩阵
    this->coupling_graph_list=coupling_list;
    this->coupling_graph_matrix=this->MakeCouplingGraph(coupling_list);
    //读取线路,确定线路需要的比特数目，这里按照qreg里面给出的数据当作需要的qubit，真实使用的可能会更少一点。
    const char * qasm_file = qasm_filename.c_str();
    this->gate_info=parse(qasm_file);
    this->circuit_num=this->qregSize[0];
    //生成门的topo序
    for(int i=0;i<this->gate_info.size();i++){
        this->gate_id_topo.push_back(i);
    }
    this->DagTable=this->generateDag(this->gate_id_topo);
}

vector<vector<int>> Environment::getNewKLayerDag(vector<int> executedgateIDs,int K)
{
    vector<vector<int>> newKLayerDag;
    for (int i = 0; i < this->circuit_num; i++) {
        vector<int> gateOnQubit;
        newKLayerDag.push_back(gateOnQubit);
    }
    vector<bool> gate_state(gate_id_topo.size(),true);
    for(int i=0;i<executedgateIDs.size();i++){
        gate_state[executedgateIDs[i]]=false;
    }
    for(int i=0;i<gate_id_topo.size();i++) {
        bool flag = gate_state[i];
        if(flag)
            continue;
        int controlQubit = this->gate_info[i].control;
        int targetQubit = this->gate_info[i].target;
        if (this->gate_info[i].control!=-1) {
            int controlQubitTime = newKLayerDag[controlQubit].size();
            int targetQubitTime = newKLayerDag[targetQubit].size();
            if(controlQubitTime == K || targetQubitTime == K) {
                break;
            }
            if (controlQubitTime > targetQubitTime) {
                for (int i = 0; i < controlQubitTime - targetQubitTime; i++) {
                    newKLayerDag[targetQubit].push_back(-1);
                }
            } else {
                for (int i = 0; i < targetQubitTime - controlQubitTime; i++) {
                    newKLayerDag[controlQubit].push_back(-1);
                }
            }
            newKLayerDag[targetQubit].push_back(i);
            newKLayerDag[controlQubit].push_back(i);
        } else {
            int targetQubitTime = newKLayerDag[targetQubit].size();
            if(targetQubitTime == K)
                break;
            newKLayerDag[targetQubit].push_back(i);
        }
        i++;
    }
    int nowDagDepth=0;
    for (int i = 0; i < newKLayerDag.size(); i++) {
        if (nowDagDepth < newKLayerDag[i].size()) {
            nowDagDepth = newKLayerDag[i].size();
        }
    }
    for (int i = 0; i < newKLayerDag.size(); i++) {
        for (int j = newKLayerDag[i].size(); j < nowDagDepth; j++) {
            newKLayerDag[i].push_back(-1);
        }
    }
    return newKLayerDag;
}


vector<vector<int>> Environment::generateDag(vector<int> gateID) {
    vector<vector<int>> newDag;
    for (int i = 0; i < this->circuit_num; i++) {
        vector<int> gateOnQubit;
        newDag.push_back(gateOnQubit);
    }

    for(int i=0;i<gateID.size();i++) {
        //cout<<"gate id is : "<<gateID[i]<<endl;
        //cout<<this->gate_info[i].type<<"  "<<this->gate_info[i].control<<" "<<this->gate_info[i].target<<endl;
        int controlQubit= this->gate_info[gateID[i]].control;
        int targetQubit= this->gate_info[gateID[i]].target;
        if (this->gate_info[gateID[i]].control!=-1) {
            int controlQubitTime = newDag[controlQubit].size();
            int targetQubitTime = newDag[targetQubit].size();
            if (controlQubitTime > targetQubitTime) {
                for (int i = 0; i < controlQubitTime - targetQubitTime; i++) {
                    newDag[targetQubit].push_back(-1);
                }
            } else {
                for (int i = 0; i < targetQubitTime - controlQubitTime; i++) {
                    newDag[controlQubit].push_back(-1);
                }
            }
            newDag[targetQubit].push_back(gateID[i]);
            newDag[controlQubit].push_back(gateID[i]);
        } else {
            newDag[targetQubit].push_back(gateID[i]);
        }
    }
    int nowDagDepth=0;
    for (int i = 0; i < newDag.size(); i++) {
        if (nowDagDepth < newDag[i].size()) {
            nowDagDepth = newDag[i].size();
        }
    }
    for (int i = 0; i < newDag.size(); i++) {
        for (int j = newDag[i].size(); j < nowDagDepth; j++) {
            newDag[i].push_back(-1);
        }
    }
    return newDag;
}


vector<vector<int>> Environment::MakeCouplingGraph(vector<vector<int>> coupling) {
    vector<int> row(this->chip_num,999);
    vector<vector<int>> couplingGraph;
    for(int i=0;i<this->chip_num;i++){
        couplingGraph.push_back(row);
    }
    for (int i=0;i<coupling.size();i++){
        int a=coupling[i][0];
        int b=coupling[i][1];
        couplingGraph[a][b]=1;
        couplingGraph[b][a]=1;
    }
    for (int i=0;i<this->chip_num;i++){
        for(int j=0;j<this->chip_num;j++){
            for(int k=0;k<this->chip_num;k++){
                if(couplingGraph[i][k]+couplingGraph[k][j]<couplingGraph[i][j]){
                    couplingGraph[i][j]=couplingGraph[i][k]+couplingGraph[k][j];
                }
            }
        }
    }
    for (int i=0;i<this->chip_num;i++){
        couplingGraph[i][i]=0;
    }
    return couplingGraph;
}


std::vector<ParsedGate> Environment::parse(const char *fileName) {
    std::ifstream infile(fileName);
    vector<ParsedGate> gates;

    char * token = 0;
    bool b = false;
    while((token = getToken(infile, b))) {//Reminder: the single = instead of double == here is intentional.

        if(!strcmp(token,"OPENQASM")) {
            token = getToken(infile,b);
            if(strcmp(token,"2.0")) {
                std::cerr << "WARNING: unexpected OPENQASM version. This may fail.\n";
            }

            assert(this->qasm_version == NULL);
            this->qasm_version = token;

            token = getToken(infile,b);
            assert(!strcmp(token,";"));
        } else if(!strcmp(token,"if")) {
            std::cerr << "FATAL ERROR: if-statements not supported.\n";
            exit(1);
        } else if(!strcmp(token,"gate")) {
            token = getCustomGate(infile);
            this->customGates.push_back(token);
        } else if(!strcmp(token,"opaque")) {
            token = getRestOfStatement(infile);
            this->opaqueGates.push_back(token);
        } else if(!strcmp(token, "include")) {
            token = getToken(infile,b);
            assert(token[0] == '"');
            this->includes.push_back(token);

            token = getToken(infile,b);
            assert(!strcmp(token,";"));
        } else if(!strcmp(token, "qreg")) {
            char * bitArray = getToken(infile, b);
            token = getToken(infile,b);
            assert(!strcmp(token,";"));

            char * temp = bitArray;
            int size = 0;
            while(*temp != '[' && *temp != 0) {
                temp++;
            }
            if(*temp == 0) {
                size = 1;
            } else {
                size = std::atoi(temp+1);
            }

            *temp = 0;
            this->qregName.push_back(bitArray);
            this->qregSize.push_back(size);
        } else if(!strcmp(token, "creg")) {
            char * bitArray = getToken(infile, b);
            token = getToken(infile,b);
            assert(!strcmp(token,";"));

            char * temp = bitArray;
            int size = 0;
            while(*temp != '[' && *temp != 0) {
                temp++;
            }
            if(*temp == 0) {
                size = 1;
            } else {
                size = std::atoi(temp+1);
            }

            *temp = 0;
            this->cregName.push_back(bitArray);
            this->cregSize.push_back(size);
        } else if(!strcmp(token, "measure")) {
            char * qbit = getToken(infile, b);

            token = getToken(infile, b);
            assert(!strcmp(token,"->"));

            char * cbit = getToken(infile, b);

            token = getToken(infile,b);
            assert(!strcmp(token,";"));

            char * temp = qbit;
            int qdx = 0;
            while(*temp != '[' && *temp != 0) {
                temp++;
            }
            if(*temp == 0) {
                qdx = 0;
            } else {
                qdx = std::atoi(temp+1);
            }
            *temp = 0;

            temp = cbit;
            int cdx = 0;
            while(*temp != '[' && *temp != 0) {
                temp++;
            }
            if(*temp == 0) {
                cdx = 0;
            } else {
                cdx = std::atoi(temp+1);
            }
            *temp = 0;

            this->measures.push_back(std::make_pair(qdx + this->getQregOffset(qbit), cdx + this->getCregOffset(cbit)));
        } else if(!strcmp(token, ";")) {
            std::cerr << "Warning: unexpected semicolon.\n";
        } else {
            char * gateName = token;
            char * qubit1Token = getToken(infile, b);
            if(strcmp(qubit1Token, ";")) {
                assert(qubit1Token && qubit1Token[0] != 0);
                int temp = 1;
                while(qubit1Token[temp] != '[' && qubit1Token[temp] != 0) {
                    temp++;
                }

                //Get flat array index corresponding to this qubit
                int originalOffset = 0;
                if(qubit1Token[temp] != 0) {
                    originalOffset = atoi(qubit1Token + temp + 1);
                }
                qubit1Token[temp] = 0;
                int qubit1FlatOffset = this->getQregOffset(qubit1Token) + originalOffset;

                char * qubit2Token = getToken(infile, b);
                if(strcmp(qubit2Token, ";")) {
                    assert(qubit2Token && qubit2Token[0] != 0);
                    int temp = 1;
                    while(qubit2Token[temp] != '[' && qubit2Token[temp] != 0) {
                        temp++;
                    }

                    //Get flat array index corresponding to this qubit
                    int originalOffset = 0;
                    if(qubit2Token[temp] != 0) {
                        originalOffset = atoi(qubit2Token + temp + 1);
                    }
                    qubit2Token[temp] = 0;
                    int qubit2FlatOffset = this->getQregOffset(qubit2Token) + originalOffset;

                    //We do not accept gates with three (or more) qubits:
                    token = getToken(infile,b);
                    assert(!strcmp(token,";"));

                    //Push this 2-qubit gate onto gate list
                    gates.push_back({gateName, qubit2FlatOffset, qubit1FlatOffset});

                } else {
                    //Push this 1-qubit gate onto gate list
                    gates.push_back({gateName, qubit1FlatOffset, -1});
                }
            } else {
                //Push this 0-qubit gate onto gate list
                gates.push_back({gateName, -1, -1});
            }
        }
    }

    return gates;
}

void Environment::PrintCoupling() {
    cout<<"coupling list :"<<endl;
    for(int i=0;i<this->coupling_graph_list.size();i++){
        cout<<coupling_graph_list[i][0]<<" "<<coupling_graph_list[i][1]<<endl;
    }
    cout<<"coupling matrix :"<<endl;
    for(int i=0;i<this->coupling_graph_matrix.size();i++){
        for(int j=0;j<this->coupling_graph_matrix[i].size();j++){
            cout<<coupling_graph_matrix[i][j]<<" ";
        }
        cout<<endl;
    }
    return ;
}



int Environment::getQregOffset(char *name) {
    int offset = 0;
    for(unsigned int x = 0; x < qregName.size(); x++) {
        if(!std::strcmp(name, qregName[x])) {
            return offset;
        } else {
            offset += qregSize[x];
        }
    }

    std::cerr << "FATAL ERROR: couldn't recognize qreg name " << name << "\n";

    assert(false);
    return -1;
}
int Environment::getCregOffset(char *name) {
    int offset = 0;
    for(unsigned int x = 0; x < cregName.size(); x++) {
        if(!std::strcmp(name, cregName[x])) {
            return offset;
        } else {
            offset += cregSize[x];
        }
    }

    assert(false);
    return -1;
}

