#ifndef ARRAY_PATH_H
#define ARRAY_PATH_H

#include "path.h"
#include "stop_point.h"
#include <string>
#include <vector>

using namespace std;

class Array_Path
{
    vector<Path *> paths;

public:
    Array_Path();
    ~Array_Path();
    Array_Path(int s);
    Array_Path(int s, int n_o);

    //START: GET e SET

    Path * get_path(Stop_Point start, Stop_Point end);

    //END: GET e SET

    void add_path   (Path * path);

};




#endif // ARRAY_PATH_H
