#include "neuron.h"
#include <iostream>
#include <limits>
#include <math.h>

Neuron::Neuron()
{
    bias = rand()%2000 - 1000;
    output = 1;
}

Neuron::Neuron(int qty_links)
{
    for(int i = 0; i < qty_links; i++){
        weights.push_back(rand()%2000 - 1000);
    }

    bias = rand()%2000 - 1000;
    output = 1;

    this->size = weights.size();
}


Neuron::~Neuron()
{
}


//START: GET e SET
int Neuron::get_size(){
    return this->size;
}

double Neuron::get_weight(int i){
    return weights[i];
}

vector<double> Neuron::get_weights(){
    return weights;
}

double Neuron::get_bias  (){
    return bias;
}

void   Neuron::set_bias  (double bias){
    this->bias = bias;
}

double Neuron::get_output(){
    return output;
}

void Neuron::set_output(double output){
    this->output = output;
}

//END: GET e SET
