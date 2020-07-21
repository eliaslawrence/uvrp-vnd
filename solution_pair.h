#ifndef SOLUTION_PAIR_H
#define SOLUTION_PAIR_H

#include <string>
#include "stop_point.h"

using namespace std;

class Solution_Pair
{
    int x, y;
    int vel;
    int charge_rate;
    double battery_charge;
public:
    Solution_Pair();
    ~Solution_Pair();

    Solution_Pair(Stop_Point s_p);
    Solution_Pair(int x, int y);
    Solution_Pair(int x, int y, int v);
    Solution_Pair(int x, int y, int v, int c);
    Solution_Pair(int x, int y, int v, int c, double b);

    //GET e SET
    int  get_x();
    void set_x(int x);

    int  get_y();
    void set_y(int y);

    int  get_vel();
    void set_vel(int v);

    int  get_charge_rate();
    void set_charge_rate(int cr);

    double  get_battery_charge();
    void set_battery_charge(double b);

    Stop_Point get_position();
    //

    void print();
    string to_string();
    bool equals(Solution_Pair e);
    bool is_at_same_position(Solution_Pair e);

    void add_vel(int v);
    void add_charge_rate(int cr);

    //Relative position
    bool is_west_from    (Solution_Pair e);
    bool is_east_from    (Solution_Pair e);
    bool is_north_from   (Solution_Pair e);
    bool is_south_from   (Solution_Pair e);
    bool is_neighbor_from(Solution_Pair e);

    Solution_Pair copy();
};


#endif // SOLUTION_PAIR_H
