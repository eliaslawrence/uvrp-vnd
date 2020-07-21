#include "layer.h"
#include <iostream>
#include <limits>
#include <math.h>

Layer::Layer()
{
}

Layer::Layer(int num_neurons)
{
    for(int i = 0; i < num_neurons; i++){
        neurons.push_back(Neuron());
    }

    this->size = neurons.size();
}

Layer::Layer(int num_neurons, int qty_links)
{
    for(int i = 0; i < num_neurons; i++){
        neurons.push_back(Neuron(qty_links));
    }

    this->size = neurons.size();
}


Layer::~Layer()
{
}


//START: GET e SET
int Layer::get_size(){
    return this->size;
}

vector<Neuron> Layer::get_neurons(){
    return neurons;
}

Neuron Layer::get_neuron(int i){
    return neurons[i];
}

//END: GET e SET
