#include "a_star_node.h"
#include "core.h"
#include <iostream>
#include <limits>

A_Star_Node::A_Star_Node()
{
}


A_Star_Node::~A_Star_Node()
{
}

A_Star_Node::A_Star_Node(A_Star_Node * parent, Stop_Point position)
{
    this->parent = parent;
    this->position = position;

    this->g = 0;
    this->h = 0;
    this->f = 0;
}

//START: GET e SET
Stop_Point A_Star_Node::get_position(){
    return this->position;
}

A_Star_Node * A_Star_Node::get_parent(){
    return this->parent;
}

double A_Star_Node::get_f(){
    return this->f;
}

void A_Star_Node::set_f(double f){
    this->f = f;
}

double A_Star_Node::get_g(){
    return this->g;
}

void A_Star_Node::set_g(double g){
    this->g = g;
}

double A_Star_Node::get_h(){
    return this->h;
}

void A_Star_Node::set_h(double h){
    this->h = h;
}

//END: GET e SET

bool A_Star_Node::equals (A_Star_Node * other){
    return this->position.same_position(other->get_position());
}

vector<A_Star_Node *> A_Star_Node::adjacents(Matrix * map, bool diagonal_movement) /* returns a vector of adjacent valid points */
{
    // Inicializando o array final
    vector<A_Star_Node *> adjacents;

    // Variavel temporaria para guardar os pontos adjacentes
    int adjacent_x = 0,
        adjacent_y = 0;

    int min_hor = -1;
    int max_hor = 1;
    int min_ver = -1;
    int max_ver = 1;

    int number_columns = map->get_cols();
    int number_lines   = map->get_rows();

    int reference_x = position.get_x();
    int reference_y = position.get_y();


    // Percorre na vertical
    for (int i = min_ver; i <= max_ver; i++)
    {
        // Percorre na horizontal
        for (int j = min_hor; j <= max_hor; j++)
        {
            // Not reference point
            if(i != 0 || j != 0){

                // Reference point added loop value
                adjacent_x = position.get_x() + j;
                adjacent_y = position.get_y() + i;

                // Inside matrix
                if (map->is_inside_perimeter(adjacent_x, adjacent_y)) {

                    // Valid movement
                    if ((!diagonal_movement && (i == 0 || j == 0)) || (diagonal_movement)) {

                        // Not prohibited
                        if(!map->is_prohibited(adjacent_x, adjacent_y)){

                            // Add node to vector
                            A_Star_Node * new_node = new A_Star_Node(this, map->get_stop_point(adjacent_x, adjacent_y));
                            adjacents.push_back(new_node);
                        }
                    }
                }
            }
        }
    }

    return adjacents;
}
