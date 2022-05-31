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
    DIR* dir = opendir(c_path);//打开指定目录
    dirent* p = NULL;//定义遍历指针
    vector<string> fileName;
    while((p = readdir(dir)) != NULL)//开始逐个遍历
    {
        //这里需要注意，linux平台下一个目录中有"."和".."隐藏文件，需要过滤掉
        if(p->d_name[0] != '.')//d_name是一个char数组，存放当前遍历到的文件名
        {
            string name = path+"/" + string(p->d_name);
            fileName.push_back(name);
            cout<<name<<endl;
        }
    }
    closedir(dir);//关闭指定目录
    return fileName;
}


int main(int argc,char *argv[]) {
    vector<int>mapping_11={0,1,2,3,4,5,6,7,8,9,10};
    vector<int>mapping_14={0,1,2,3,4,5,6,7,8,9,10,11,12,13};
    ofstream outputFile;
    string readfile("/home/qtest/QuantumCompiler/circuits/large/");
    string outputf("../");
    readfile.append(argv[1]);
    outputf.append(argv[2]);
    outputFile.open(outputf,ios::app);
    clock_t startTime,endTime;
    vector<vector<int>> coupling_map_list_1x11={{0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8},{8,9},{9,10}};
    vector<vector<int>> coupling_map_list_mel={{0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,8},{7,8},{8,9},{9,10},{10,11},{11,12},{12,13},{1,13},{2,12},{3,11},{4,10},{5,9}};
    //Environment *env = new Environment(coupling_map_list_1x11,readfile);
    Environment *env = new Environment(coupling_map_list_1x11,"/home/qtest/QuantumCompiler/circuits/tt11.qasm");
    env->PrintCoupling();
    startTime=clock();
    Search *sr = new Search(env);
    SearchResult a= sr->SearchSmoothWithInitialMappingAdpat(mapping_11,5000);
    endTime=clock();
    outputFile<<readfile<<endl;
    for (int i = 0; i < a.finalPath.size(); i++) {
        for (int j = 0; j < a.finalPath[i].actions.size(); j++) {
            outputFile << a.finalPath[i].actions[j].gateID << " " << a.finalPath[i].actions[j].gateName << " "
                       << a.finalPath[i].actions[j].controlQubit << " " << a.finalPath[i].actions[j].targetQubit << "   ";
        }
        outputFile << endl;
    }
    int count = 0;
    for (int i = 0; i < a.searchNodeNum.size(); i++) {
        count = count + a.searchNodeNum[i];
    }
    outputFile << "search node number: " << count << endl;
    int pattern = 0;
    for (int i = 0; i < a.finalPath.size(); i++) {
        if (a.finalPath[i].pattern == true) {
            pattern++;
        }
    }
    outputFile << "pattern number: " << pattern << endl;
    outputFile << "cycle num is " << a.cycleNum << endl;
    outputFile << "how many path has done: " << a.finalPath.size() << endl;
    outputFile<<"time is "<<(double)(endTime-startTime)/CLOCKS_PER_SEC<<endl;
    outputFile<<endl<<endl;
    return 0;
}
