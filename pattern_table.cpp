#include "pattern_table.h"
#include "core.h"
#include <iostream>

using namespace fixedValues;

Pattern_Table::~Pattern_Table()
{
//    delete [] routes;
//    delete [] points;
    routes.clear();
    points.clear();
}

Pattern_Table::Pattern_Table()
{
//    this->points_size      = 0;
//    this->routes_size      = 0;
//    this->routes           = new int[1];
//    this->points           = new Solution_Pair[1];
}

void Pattern_Table::resize_routes(int s)
{
//    this->routes_size = s;
//    int *new_data = new int[s];

//    for (int i = 0; i < s; ++i) {
//        new_data[i] = routes[i];
//    }

//    delete [] routes;

//    routes = new_data;
}

void Pattern_Table::resize_points(int s)
{
//    this->points_size = s;
//    Solution_Pair *new_data = new Solution_Pair[s];

//    for (int i = 0; i < s; ++i) {
//        new_data[i] = points[i].copy();
//    }

//    delete [] points;
//    points = new Solution_Pair[s];

//    for (int i = 0; i < s; ++i) {
//        points[i] = new_data[i].copy();
//    }
//    delete [] new_data;
}

int Pattern_Table::get_routes_size() {
    return routes.size();//routes_size;
}

int Pattern_Table::get_points_size() {
    return points.size();//points_size;
}

Solution_Pair Pattern_Table::get_point(int pos)
{
   return points[pos];
}

int Pattern_Table::get_route(int pos)
{
    return routes[pos];
}

void Pattern_Table::add_point(int x, int y)
{

    Solution_Pair s_p(x, y);
    points.push_back(s_p);
//    points_size++;
//    resize_points(points_size);

//    Solution_Pair s_p(x, y);
//    points[points_size - 1] = s_p;
}

void Pattern_Table::add_point(int x, int y, int v)
{

    Solution_Pair s_p(x, y, v);
    points.push_back(s_p);
}

void Pattern_Table::add_point(Solution_Pair p)
{    
    points.push_back(p.copy());
//    points_size++;
//    resize_points(points_size);

//    points[points_size - 1] = p.copy();
}

void Pattern_Table::add_route(int route)
{    
    routes.push_back(route);
//    routes_size++;
//    resize_routes(routes_size);

//    routes[routes_size - 1] = route;
}

void Pattern_Table::print_points()
{
    for (int i = 0; i < points.size(); i++) {
        points[i].print();
        cout << ": " << points[i].get_vel() << " ";
    }

    cout << endl;
}

void Pattern_Table::print()
{
    for (int i = 0; i < points.size(); i++) {
        points[i].print();
        cout << " ";
    }

    cout << ": ";

    for (int i = 0; i < routes.size(); i++) {
        cout << routes[i] << ", ";
    }

    cout << endl;
}

string Pattern_Table::to_string()
{
    string solution;

    for (int i = 0; i < points.size(); i++) {
        solution.append(points[i].to_string());
        solution.append(" ");
    }

    solution.append(": ");

    for (int i = 0; i < routes.size(); i++) {
        solution.append(std::to_string(routes[i]));
        solution.append(", ");
    }

    return solution;
}

bool Pattern_Table::can_join_sequence(Pattern_Table other)
{
    for (int i = 0; i < points.size() - 1; i++) {
        if(!points[i+1].equals(other.get_point(i))){
            return false;
        }
    }

    return true;
}

bool Pattern_Table::can_join(Pattern_Table other)
{
    for (int i = 0; i < points.size() - 1; i++) {
        if(!points[i].is_at_same_position(other.get_point(i))){
            return false;
        }
    }

    return true;
}

bool Pattern_Table::equals_but(Pattern_Table other, int index)
{
    int i = 0;
    for (int j = 0; j < other.get_points_size(); j++) {
        if(j != index){
            if(!points[i].is_at_same_position(other.get_point(j))){
                return false;
            }
            i++;
        }
    }

    return true;
}

bool Pattern_Table::is_subsequence(Pattern_Table other)
{
    int last_pos = 0;
    bool found;
    for (int i = 0; i < this->get_points_size(); i++) {
        found = false;
        for (int j = last_pos; j < other.get_points_size(); j++) {
            if(points[i].is_at_same_position(other.get_point(j))){
                found = true;
                last_pos = j + 1;
                break;
            }
        }

        if(!found){
            return false;
        }
    }

    return true;
}

void Pattern_Table::copy(Pattern_Table other)
{
    for (int i = 0; i < other.get_points_size(); i++) {
        add_point(other.get_point(i));
    }

    for (int i = 0; i < other.get_routes_size(); i++) {
        add_route(other.get_route(i));
    }
}


void Pattern_Table::insert_union(Pattern_Table table1, Pattern_Table table2)
{
    int i = 0;
    for (i = 0; i < table1.get_points_size(); i++) {
        add_point(table1.get_point(i));
    }

    add_point(table2.get_point(i-1));

    for (i = 0; i < table1.get_routes_size(); i++) {
        for (int j = 0; j < table2.get_routes_size(); j++) {
            if(table1.get_route(i) == table2.get_route(j)){
                add_route(table1.get_route(i));
                break;
            }
        }
    }
}
