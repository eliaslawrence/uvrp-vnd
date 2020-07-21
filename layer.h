#ifndef LAYER_H
#define LAYER_H

using namespace std;

#include "neuron.h"
#include <vector>

class Layer
{
    int size;


public:
    Layer();
    Layer(int num_neurons);
    Layer(int num_neurons, int qty_links);

    ~Layer();

    vector<Neuron> neurons;

    //GET and SET
    int            get_size();

    vector<Neuron> get_neurons();

    Neuron         get_neuron(int i);
    //

    double output ();

};

#endif // LAYER_H
