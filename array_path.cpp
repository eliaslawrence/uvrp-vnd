#include "array_path.h"
#include "core.h"
#include <iostream>
#include <limits>

using namespace fixedValues;

Array_Path::Array_Path()
{
}


Array_Path::~Array_Path()
{
    int size = this->paths.size();
    for (int var = 0; var < size; var++) {
        delete this->paths[var];
    }
}

Path * Array_Path::get_path (Stop_Point start, Stop_Point end)
{
    Path * path;
    for (int i = 0; i < this->paths.size(); i++) {
        path = this->paths[i];
        if(start.same_position(path->get_start()) && end.same_position(path->get_end())){
           return path;
        }
    }

    return path;
}

void Array_Path::add_path (Path * path)
{
    this->paths.push_back(path->copy());
}


