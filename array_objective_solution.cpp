#include "array_objective_solution.h"
#include "core.h"
#include <iostream>
#include <limits>

using namespace fixedValues;

Array_Objective_Solution::Array_Objective_Solution()
{
}


Array_Objective_Solution::~Array_Objective_Solution()
{
    for (int var = 0; var < this->solutions.size(); var++) {
        delete this->solutions[var];
    }
}

void Array_Objective_Solution::add(Objective_Solution * o_s)
{
    for (int i = 0; i < this->solutions.size(); i++)
    {
        if(o_s->dominates(this->solutions[i])){
            this->solutions.erase(this->solutions.begin() + i);
            i--;
        }else if(this->solutions[i]->dominates(o_s) || this->solutions[i]->equals(o_s)){
            return;
        }
    }

    this->solutions.push_back(o_s->copy());
}

int Array_Objective_Solution::size()
{
    return this->solutions.size();
}

Objective_Solution * Array_Objective_Solution::get(int i)
{
    return this->solutions[i];
}

void Array_Objective_Solution::print()
{
    // Print the objective values
    for (int i = 0; i < this->solutions.size(); i++) {
        this->solutions[i]->print();
        cout << "\n";
    }
}

void Array_Objective_Solution::to_file(string file_name)
{
    ofstream aggregate_file;
    aggregate_file.open (file_name);

    aggregate_file << this->solutions.size() << " " << this->solutions[0]->get_num_objectives() << "\n";


    for (int var = 0; var < this->solutions.size(); var++) {
        for (int obj = 0; obj < this->solutions[var]->get_num_objectives(); obj++) {
            aggregate_file << this->solutions[var]->get_z()[obj] << " ";
        }

        aggregate_file <<  "\n";
    }

    aggregate_file.close();
}

void Array_Objective_Solution::from_file(string file_name)
{
    ifstream aggregate_file;
    aggregate_file.open (file_name);

    int n_sol, n_obj;

    aggregate_file >> n_sol;
    aggregate_file >> n_obj;

    Objective_Solution * o_s;

    double z;

    for (int s = 0; s < n_sol; s++)
    {
        o_s = new Objective_Solution(n_obj);
        for (int o = 0; o < n_obj; o++)
        {
            aggregate_file >> z;
            o_s->set_z(o, z);
        }

        this->add(o_s);
        delete o_s;
    }

    aggregate_file.close();
}
