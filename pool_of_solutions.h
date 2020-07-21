#ifndef POOL_OF_SOLUTIONS_H
#define POOL_OF_SOLUTIONS_H

#include "solution_pair.h"
#include "array_solution.h"
#include <string>
#include <vector>

using namespace std;

class Pool_of_Solutions
{
    int size;
    int max_size;


public:
    vector<Array_Solution> solutions; // pointer

    Pool_of_Solutions();
    ~Pool_of_Solutions();
    Pool_of_Solutions(int s);

    //START: get/set

    int               get_size();
    void              set_size(int s);

    vector<Array_Solution> get_solutions(); // pointer

    //END: get/set

    bool add_solution   (Array_Solution a); // pointer
    bool add            (Array_Solution a); // pointer
    void remove_solution(int pos);
    void print          ();
    void clean          ();

    string to_string    ();
};




#endif // POOL_OF_SOLUTION_H
