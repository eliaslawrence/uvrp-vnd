#ifndef ARRAY_OBJECTIVE_SOLUTION_H
#define ARRAY_OBJECTIVE_SOLUTION_H

#include "solution_pair.h"
#include "objective_solution.h"
#include <string>
#include <vector>

using namespace std;

class Array_Objective_Solution
{        
    vector<Objective_Solution *> solutions;

public:
    Array_Objective_Solution();
    ~Array_Objective_Solution();

    //START: GET e SET
    //

    void add(Objective_Solution * o_s);    
    void print();
    void to_file(string file_name);
    void from_file(string file_name);

    int size();

    Objective_Solution * get(int i);
};




#endif // ARRAY_OBJECTIVE_SOLUTION_H
