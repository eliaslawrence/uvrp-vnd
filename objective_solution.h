#ifndef OBJECTIVE_SOLUTION_H
#define OBJECTIVE_SOLUTION_H

#include "solution_pair.h"
#include <string>
#include <vector>

using namespace std;

class Objective_Solution
{    
    double *       z; //0: deliver/collect, 1: final charge, 2: time, 3: consumption
    int            num_objectives;

    void initialize_z(int n_obj);

public:
    Objective_Solution();
    ~Objective_Solution();
    Objective_Solution(int n_o);

    //START: GET e SET
    double * get_z();
    void     set_z(int pos, double value);

    int get_num_objectives();
    //

    Objective_Solution * copy();

    void print();

    bool equals          (Objective_Solution * s_2);
    bool dominates       (Objective_Solution * s_2);
    bool weakly_dominates(Objective_Solution * s_2);
};




#endif // OBJECTIVE_SOLUTION_H
