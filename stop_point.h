#ifndef STOP_POINT_H
#define STOP_POINT_H

using namespace std;

class Stop_Point
{
    int x, y, type;
public:
    Stop_Point();
    ~Stop_Point();
    Stop_Point(int x, int y, int type);
    Stop_Point(int x, int y);

    //GET e SET
    int  get_x();
    void set_x(int x);

    int  get_y();
    void set_y(int y);

    int  get_type();
    void set_type(int type);
    //

    void print();
    bool equals(Stop_Point s);
    bool equals(int type);
    bool same_position(Stop_Point s);

    Stop_Point copy();
};

#endif // STOP_POINT_H
