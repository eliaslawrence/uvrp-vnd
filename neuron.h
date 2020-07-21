#ifndef NEURON_H
#define NEURON_H

using namespace std;

#include <vector>

class Neuron
{    

    int    size;
    double bias;
    double output;

public:
    Neuron();
    Neuron(int qty_links);

    ~Neuron();

    vector<double> weights;

    //GET and SET
    int            get_size  ();

    double         get_weight(int i);
    vector<double> get_weights();

    double         get_bias  ();
    void           set_bias  (double bias);

    double         get_output();
    void           set_output (double output);
    //
};

#endif // NEURON_H
