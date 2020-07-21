#include "pool_of_solutions.h"
#include "core.h"
#include <iostream>

using namespace fixedValues;

Pool_of_Solutions::Pool_of_Solutions()
{
}


Pool_of_Solutions::~Pool_of_Solutions()
{
//    for (int var = 0; var < size; var++) {
//        delete this->solutions[var];
//    }
}

Pool_of_Solutions::Pool_of_Solutions(int m_s)
{
    this->size      = 0;
    this->max_size  = 20;//m_s;
    this->solutions.assign(20, Array_Solution(3000));
//    this->solutions = new Array_Solution * [m_s];
//    this->solutions.resize(this->max_size);
}

void Pool_of_Solutions::clean(){
    this->size      = 0;
    this->max_size  = 20;


//    vector<Array_Solution> ().swap(solutions);
    solutions.clear();
//    while(!solutions.empty()){
//        solutions.pop_back();
//    }

//    for (unsigned i = 0; i < solutions.size(); i++) {
//        remove_solution(i);
//    }

//    v.swap(solutions);
}

int Pool_of_Solutions::get_size() {
    return this->size;//solutions.size();
}

void Pool_of_Solutions::set_size(int s) {
    this->size = s;
}

vector<Array_Solution> Pool_of_Solutions::get_solutions(){ // pointer
    return this->solutions;
}

bool Pool_of_Solutions::add_solution(Array_Solution a) // pointer
{
//   if(!a->is_valid()){
//       return false;
//   }

   for (int i = 0; i < this->size; i++)
   {
       if(a.equals(this->solutions[i])){
           return false;
       }else if(a.dominates(this->solutions[i])){
           this->remove_solution(i);
           i--;
       }else if(this->solutions[i].dominates(a)){
           return false;
       }
   }

   return this->add(a);
//   return true;
}

bool Pool_of_Solutions::add(Array_Solution a) // pointer
{
//    double minimum_points = a->get_points();
//    int pos_minimum = -1;

    if(this->size == this->max_size){
        return false;

//        this->max_size++;
//        this->solutions.resize(this->max_size);


//        for (int i = 0; i < this->size; i++)
//        {
//            if(this->solutions[i]->get_points() < minimum_points){
//                minimum_points = this->solutions[i]->get_points();
//                pos_minimum = i;
//            }
//        }

//        //If the solution that I want to insert it is not the minimal one
//        if(pos_minimum != -1){
//            //Remove the minimal solution
//            this->remove_solution(pos_minimum);
//        }else{
//            return false;
//        }
    }

    solutions[size] = a.full_copy();
//    solutions.push_back(a.full_copy());
//    solutions.push_back(Array_Solution(a));


//    this->solutions[this->size] = a->copy();
    this->size++;
    return true;
}

void Pool_of_Solutions::remove_solution(int pos)
{   
//   delete this->solutions[pos];
//   solutions.erase(solutions.begin() + pos);
//   vector<Array_Solution> temp;

    Array_Solution temp;


    for (int i = pos; i < this->size - 1; i++)
    {
        temp = this->solutions[i];
        this->solutions[i] = this->solutions[i+1];
        this->solutions[i+1] = temp;
    }

    this->solutions.pop_back();

//   for (unsigned i = 0; i < this->solutions.size(); i++)
//   {
//       if(i != pos){
//           temp.push_back(this->solutions[i].full_copy());
//       }
//   }

//   temp.swap(solutions);
//   temp.clear();

//   for (int i = pos; i < this->size - 1; i++)
//   {
//       this->solutions[i] = this->solutions[i+1];
//   }

   this->size--;
}

void Pool_of_Solutions::print()
{
    cout << "\n\nPOOL of SOLUTIONS: \n" << endl;

    // Imprime o caminho percorrido pelo drone
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < solutions[i].get_num_objectives(); j++) {
            cout << "z" << j << ": "<< solutions[i].get_z()[j] << " | ";
        }

        solutions[i].print();
    }
}

string Pool_of_Solutions::to_string()
{
    // Imprime o caminho percorrido pelo drone

    string solution;
    solution.append("[");

    for (int i = 0; i < size; i++) {
        solution.append(solutions[i].to_string());
    }

    solution.append("]");

    return solution;
}
