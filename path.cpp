#include "array_path.h"
#include "core.h"
#include <iostream>
#include <limits>

using namespace fixedValues;

Path::Path()
{
}

Path::Path(Stop_Point start, Stop_Point end, Array_Solution path)
{
    this->start = start;
    this->end = end;
    this->path = path;
}


Path::~Path()
{
//    delete this->path;
}

Stop_Point Path::get_start()
{
    return this->start;
}

void Path::set_start(Stop_Point start)
{
    this->start = start.copy();
}

Stop_Point Path::get_end()
{
    return this->end;
}

void Path::set_end(Stop_Point end)
{
    this->end = end.copy();
}

Array_Solution Path::get_path()
{
    return this->path;
}

Path * Path::copy()
{
    Path * copy = new Path(this->get_start().copy(), this->get_end().copy(), this->get_path().copy());
    return copy;
}


