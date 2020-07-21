#include "stop_point.h"
#include <iostream>


Stop_Point::Stop_Point()
{
    this->x    = 0;
    this->y    = 0;
    this->type = 0;
}


Stop_Point::~Stop_Point()
{
}

Stop_Point::Stop_Point(int x, int y)
{
    this->x    = x;
    this->y    = y;
    this->type = 0;
}

Stop_Point::Stop_Point(int x, int y, int type)
{
    this->x    = x;
    this->y    = y;
    this->type = type;
}

int Stop_Point::get_x()
{
    return x;
}

void Stop_Point::set_x(int x)
{
    this->x = x;
}

int Stop_Point::get_y()
{
    return y;
}

void Stop_Point::set_y(int y)
{
    this->y = y;
}

int Stop_Point::get_type()
{
    return this->type;
}

void Stop_Point::set_type(int type)
{
    this->type = type;
}

void Stop_Point::print()
{
    cout << this->type << " ";
}

bool Stop_Point::equals(Stop_Point e)
{
    return (e.get_type() == this->type);
}

bool Stop_Point::equals(int type)
{
    return (type == this->type);
}

bool Stop_Point::same_position(Stop_Point e)
{
    return (e.get_x() == this->get_x() && e.get_y() == this->get_y());
}

Stop_Point Stop_Point::copy()
{
    Stop_Point copy(this->x,this->y, this->type);
    return copy;
}
