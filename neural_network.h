#ifndef NEURAL_NETWORK_H
#define NEURAL_NETWORK_H

using namespace std;

#include "layer.h"
#include "solution_pair.h"
#include <vector>

class Neural_Network
{    
    Layer output_layer;
    /*
     * OUTPUTS:
     *
     * 00 - + OR - (pos x)
     * 01 - 1 OR 0 (pos x)
     * 02 - + OR - (pos y)
     * 03 - 1 OR 0 (pos y)
     * 04 - VEL [1, 10]
     * 05 - CHARGE RATE [0, 100]
     * 06 - STOP (TRUE or FALSE)
     */

    Layer entry_layer;
    /*
     * ENTRY:
     *
     * 00 - POS X
     * 01 - POS Y
     * 02 - VEL
     * 03 - BATTERY CHARGE
     * 04 - NUM CLIENTS VISITED
     * 05 - TYPE OF POINT (RECHARGE, CLIENT, PROHIBITED)
     */

public:    
    Neural_Network();
    Neural_Network(int num_layers_hid, int num_neurons_entry, int num_neurons_hid, int num_neurons_out);
    ~Neural_Network();

    vector<Layer> hidden_layers;

    //GET and SET
    Layer get_output_layer();

    void set_entry  (Solution_Pair pair, int num_clients, int type);
    void set_weights(vector<double> dna);
    //

    double calculate_output ();
    double hidden_activation(double y);
    double output_activation(int output_type, double y);

};

#endif // NEURAL_NETWORK_H
