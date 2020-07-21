#ifndef A_START_NODE_H
#define A_START_NODE_H

#include "stop_point.h"
#include "matrix.h"
#include <vector>

using namespace std;

class A_Star_Node
{
    Stop_Point    position;
    A_Star_Node * parent;
    double        f, g, h;

public:
    A_Star_Node();
    ~A_Star_Node();
    A_Star_Node(A_Star_Node * parent, Stop_Point position);

    //START: GET e SET
    Stop_Point get_position();

    A_Star_Node * get_parent();

    double get_f();
    void set_f(double f);

    double get_g();
    void set_g(double g);

    double get_h();
    void set_h(double h);
    //END: GET e SET

    bool equals (A_Star_Node * other);

    vector<A_Star_Node *> adjacents(Matrix * map, bool diagonal_movement);
};




#endif // A_START_NODE_H
