#ifndef HYPERVOLUME_H
#define HYPERVOLUME_H

#include <string>
//#include "string.h"
#include <vector>
#include <cstring>
#include <sstream>
#include "array_objective_solution.h"

using namespace std;

class Hypervolume
{

private:
    static char* execCommand(const char* command);

public:
    Hypervolume();
    ~Hypervolume();

    //START: GET e SET
    //

    static double hypervolumeWithExecRequestedbyFile(string pareto_file_name, vector<double> refPoints);
    static double hypervolumeWithExecRequested      (vector<vector<double>> pareto, vector<double> refPoints, bool minimization);
    static double setCoverage                       (Array_Objective_Solution * a, Array_Objective_Solution * b);

    static int    cardinalite                       (Array_Objective_Solution * D, Array_Objective_Solution * ref);
};




#endif // HYPERVOLUME_H
