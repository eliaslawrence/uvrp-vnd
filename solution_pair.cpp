#include "solution_pair.h"
#include <iostream>


Solution_Pair::Solution_Pair()
{
}


Solution_Pair::~Solution_Pair()
{
}

Solution_Pair::Solution_Pair(Stop_Point s_p)
{
    this->x = s_p.get_x();
    this->y = s_p.get_y();

    this->vel = 10;
    this->charge_rate = 100;
    this->battery_charge = 100;
}

Solution_Pair::Solution_Pair(int x, int y)
{
    this->x = x;
    this->y = y;

    this->vel = 10;
    this->charge_rate = 100;
    this->battery_charge = 100;
}

Solution_Pair::Solution_Pair(int x, int y, int v)
{
    this->x = x;
    this->y = y;

    //Velocity
    if (v < 1){
        this->vel = 1;
    } else if (v > 10){
        this->vel = 10;
    } else{
        this->vel = v;
    }

    //Charge Rate
    this->charge_rate = 100;

    //Battery Charge
    this->battery_charge = 100;
}

Solution_Pair::Solution_Pair(int x, int y, int v, int c)
{
    this->x = x;
    this->y = y;

    //Velocity
    if (v < 1){
        this->vel = 1;
    } else if (v > 10){
        this->vel = 10;
    } else{
        this->vel = v;
    }

    //Charge Rate
    if (c < 0){
        this->charge_rate = 0;
    } else if (c > 100){
        this->charge_rate = 100;
    } else{
        this->charge_rate = c;
    }

    //Battery Charge
    this->battery_charge = 100;    
}

Solution_Pair::Solution_Pair(int x, int y, int v, int c, double b)
{
    this->x = x;
    this->y = y;

    //Velocity
    if (v < 1){
        this->vel = 1;
    } else if (v > 10){
        this->vel = 10;
    } else{
        this->vel = v;
    }

    //Charge Rate
    if (c < 0){
        this->charge_rate = 0;
    } else if (c > 100){
        this->charge_rate = 100;
    } else{
        this->charge_rate = c;
    }

    //Battery Charge
    if (b < 0){
        this->battery_charge = 0;
    } else if (b > 100){
        this->battery_charge = 100;
    } else{
        this->battery_charge = b;
    }
}

int Solution_Pair::get_x()
{
    return x;
}

void Solution_Pair::set_x(int x)
{
    this->x = x;
}

int Solution_Pair::get_y()
{
    return y;
}

void Solution_Pair::set_y(int y)
{
    this->y = y;
}

int  Solution_Pair::get_vel(){
    return this->vel;
}

void Solution_Pair::set_vel(int v){
    this->vel = v;
}

void Solution_Pair::add_vel(int v){
    this->vel += v;
}

int  Solution_Pair::get_charge_rate(){
    return this->charge_rate;
}

void Solution_Pair::set_charge_rate(int cr){
    this->charge_rate = cr;
}

void Solution_Pair::add_charge_rate(int cr){
    this->charge_rate += cr;
}

double Solution_Pair::get_battery_charge(){
    return this->battery_charge;
}

void Solution_Pair::set_battery_charge(double b){
    this->battery_charge = b;
}

void Solution_Pair::print()
{
    cout << "(" << x << "," << y << ") ";
}

string Solution_Pair::to_string()
{    
    string resp;

    resp.append("(");
    resp.append(std::to_string(x));
    resp.append(",");
    resp.append(std::to_string(y));
    resp.append(") ");

    return resp;
}

bool Solution_Pair::equals(Solution_Pair e)
{
    return (e.get_x() == this->x && e.get_y() == this->y && e.get_vel() == this->vel && e.get_charge_rate() == this->charge_rate);
}

bool Solution_Pair::is_at_same_position(Solution_Pair e)
{
    return (e.get_x() == this->x && e.get_y() == this->y);
}

bool Solution_Pair::is_west_from(Solution_Pair e)
{
    return (this->x < e.get_x());
}

bool Solution_Pair::is_east_from(Solution_Pair e)
{
    return (this->x > e.get_x());
}

bool Solution_Pair::is_north_from(Solution_Pair e)
{
    return (this->y < e.get_y());
}

bool Solution_Pair::is_south_from(Solution_Pair e)
{
    return (this->y > e.get_y());
}

bool Solution_Pair::is_neighbor_from(Solution_Pair e)
{
    return (abs(this->x - e.get_x()) <= 1 && abs(this->y - e.get_y()) <= 1 && !is_at_same_position(e));
}

Solution_Pair Solution_Pair::copy()
{
    Solution_Pair copy(this->x,this->y, this->vel, this->charge_rate, this->battery_charge);
    return copy;
}

Stop_Point Solution_Pair::get_position(){
    Stop_Point pos(this->get_x(), this->get_y());
    return pos;
}
