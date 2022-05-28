#include <iostream>
#include <time.h>
#include "Environment/Environment.h"
#include "Search/SearchNode.h"
#include "Search/Search.h"
#include <dirent.h>
#include <fstream>

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

vector<string> GetFileName(string a){
    string path="/home/qtest/QuantumCompiler/circuits/large/"+a;
    const char* c_path=path.c_str();
    DIR* dir = opendir(c_path);//��ָ��Ŀ¼
    dirent* p = NULL;//�������ָ��
    vector<string> fileName;
    while((p = readdir(dir)) != NULL)//��ʼ�������
    {
        //������Ҫע�⣬linuxƽ̨��һ��Ŀ¼����"."��".."�����ļ�����Ҫ���˵�
        if(p->d_name[0] != '.')//d_name��һ��char���飬��ŵ�ǰ���������ļ���
        {
            string name = path+"/" + string(p->d_name);
            fileName.push_back(name);
            cout<<name<<endl;
        }
    }
    closedir(dir);//�ر�ָ��Ŀ¼
    return fileName;
}


int main() {
    vector<int>mapping_11={0,1,2,3,4,5,6,7,8,9,10,11};

//    outputFile.open("../11_1x11_adapt_new.txt",ios::app);
    vector<vector<int>> coupling_map_list_1x11={{0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8},{8,9},{9,10}};
    Environment *env = new Environment(coupling_map_list_1x11,"/home/qtest/QuantumCompiler/circuits/large/11/z4_268.qasm");
    //Environment *env = new Environment(coupling_map_list_1x11,"/home/qtest/QuantumCompiler/circuits/tt11.qasm");
    env->PrintCoupling();
    Search *sr = new Search(env);
    SearchResult a= sr->SearchSmoothWithInitialMappingAdpat(mapping_11,1200);

    return 0;
}
