#include "objective_solution.h"
#include "core.h"
#include <iostream>
#include <limits>

using namespace fixedValues;

Objective_Solution::Objective_Solution()
{
}


Objective_Solution::~Objective_Solution()
{
    delete [] z;
}

Objective_Solution::Objective_Solution(int n_obj)
{    
    this->num_objectives = n_obj;
    initialize_z(this->num_objectives);
}

void Objective_Solution::initialize_z(int n_obj)
{
    z = new double[n_obj];

    for (int i = 0; i < this->num_objectives; i++) {
        z[i] = 0;
    }
}

double* Objective_Solution::get_z(){
    return this->z;
}

void Objective_Solution::set_z(int pos, double value){
    this->z[pos] = value;
}

int Objective_Solution::get_num_objectives(){
    return this->num_objectives;
}

bool Objective_Solution::equals(Objective_Solution * s_2) /* verify if solution is equal to another*/
{
    // Percorre todo array
    for (int i = 0; i < num_objectives; i++)
    {
        // Compara o ponto de referencia com os pares ordenados do array
        if (this->get_z()[i] != s_2->get_z()[i]) {
            return false;
        }
    }

    return true;
}

bool Objective_Solution::dominates(Objective_Solution * s_2) /* verifica se solucao domina outra solucao */
{    
    bool greaterThan = false;

    // Percorre todo array
    for (int i = 0; i < num_objectives; i++)
    {
        // Compara o ponto de referencia com os pares ordenados do array
        if (this->get_z()[i] < s_2->get_z()[i]) {
            return false;
        }else if(this->get_z()[i] > s_2->get_z()[i]){
            greaterThan = true;
        }
    }
//    return greaterThan ? greaterThan : this->size < a_2->size;
    return greaterThan;
}


bool Objective_Solution::weakly_dominates(Objective_Solution * s_2) /* verifica se solucao domina outra solucao */
{    
    // Percorre todo array
    for (int i = 0; i < num_objectives; i++)
    {
        // Compara o ponto de referencia com os pares ordenados do array
        if (this->get_z()[i] < s_2->get_z()[i]) {
            return false;
        }
    }

    return true;
}

Objective_Solution * Objective_Solution::copy()
{
    Objective_Solution * copy = new Objective_Solution(this->num_objectives);

    for (int i = 0; i < this->num_objectives; i++) {
        copy->set_z(i, this->z[i]);
    }

    return copy;
}

void Objective_Solution::print()
{
    // Print the objective values
    for (int i = 0; i < this->num_objectives; i++) {
        cout << this->z[i] << " ";
    }
}
