#ifndef PATH_H
#define PATH_H

#include "solution_pair.h"
#include "array_solution.h"
#include <string>
#include <vector>

using namespace std;

class Path
{

    Stop_Point       start;
    Stop_Point       end;
    Array_Solution   path;

public:
    Path();
    Path(Stop_Point start, Stop_Point end, Array_Solution path);
    ~Path();

    //START: GET e SET

    Stop_Point get_start();
    void       set_start(Stop_Point start);

    Stop_Point get_end();
    void       set_end(Stop_Point end);

    Array_Solution get_path();

    //END: GET e SET

    Path * copy();

};




#endif // PATH_H
