#include <iostream>
#include "Environment/Environment.h"

int main() {
    string qasm_filename="D:\\study\\github\\sMapper\\circuits\\small\\test1.qasm";
    vector<vector<int>>  coupling_map_list_qx2 = {{0, 1}, {0, 2}, {1, 2}, {2, 3}, {2, 4}, {3, 4}};
    Environment env(coupling_map_list_qx2,qasm_filename);
    for(int i=0;i<env.gate_info.size();i++){
        cout<<env.gate_info[i].type<<" "<<env.gate_info[i].control<<" "<<env.gate_info[i].target<<endl;
    }
    std::cout << "Hello, World!" << std::endl;
    for(int i=0;i<env.qregSize.size();i++){
        cout<<env.qregSize[i]<<endl;
    }
    return 0;
}
