#include "neural_network.h"
#include <iostream>
#include <limits>
#include <math.h>

Neural_Network::Neural_Network()
{
}

Neural_Network::Neural_Network(int num_layers_hid, int num_neurons_entry, int num_neurons_hid, int num_neurons_out)
{
    entry_layer = Layer(num_neurons_entry);

    hidden_layers.push_back(Layer(num_neurons_hid, num_neurons_entry));

    for(int i = 1; i < num_layers_hid; i++){
        hidden_layers.push_back(Layer(num_neurons_hid, num_neurons_hid));
    }

    output_layer = Layer(num_neurons_out, num_neurons_hid);

}


Neural_Network::~Neural_Network()
{
}


//START: GET e SET
Layer Neural_Network::get_output_layer(){
    return output_layer;
}

void Neural_Network::set_entry(Solution_Pair pair, int num_clients, int type){
    entry_layer.neurons[0].set_output(pair.get_x());
    entry_layer.neurons[1].set_output(pair.get_y());
    entry_layer.neurons[2].set_output(pair.get_vel());
    entry_layer.neurons[3].set_output(pair.get_charge_rate());
    entry_layer.neurons[4].set_output(num_clients);
    entry_layer.neurons[5].set_output(type);
}

void Neural_Network::set_weights(vector<double> dna){
    int pos_dna = 0;

    // For each layer
    for(int i = 0; i < hidden_layers.size(); i++){
        // For each neuron
        for(int j = 0; j < hidden_layers[i].get_size(); j++){
            // For each weight
            for(int k = 0; k < hidden_layers[i].neurons[k].get_size(); k++){
                hidden_layers[i].neurons[j].weights[k] = dna[pos_dna];
                pos_dna++;
            }

            hidden_layers[i].neurons[j].set_bias(dna[pos_dna]);
            pos_dna++;
        }
    }

    // For each neuron
    for(int j = 0; j < output_layer.get_size(); j++){
        // For each weight
        for(int k = 0; k < output_layer.neurons[k].get_size(); k++){
            output_layer.neurons[j].weights[k] = dna[pos_dna];
            pos_dna++;
        }

        output_layer.neurons[j].set_bias(dna[pos_dna]);
        pos_dna++;
    }
}

//END: GET e SET

double Neural_Network::calculate_output(){
    double output = 0;

    // First hidden layer output from entry layer
    // For each neuron
    for(int i = 0; i < hidden_layers[0].get_size(); i++){
        output = 0;
        //For each weight
        for(int j = 0; j < hidden_layers[0].get_neuron(i).get_size(); j++){
            output = output + hidden_layers[0].get_neuron(i).get_weight(j) * entry_layer.get_neuron(j).get_output();
        }

        output = output + hidden_layers[0].get_neuron(i).get_bias();
        hidden_layers[0].neurons[i].set_output(hidden_activation(output));
    }
    /* ----------------------------------------------------------------------------------------------------------- */

    // Other hidden layers output from previous hidden layer
    // For each hidden layer
    for(int i = 1; i < hidden_layers.size(); i++){
        //For each neuron
        for(int j = 0; j < hidden_layers[i].get_size(); j++){
            output = 0;
            //For each weight
            for(int k = 0; k < hidden_layers[i].get_neuron(j).get_size(); k++){
                output = output + hidden_layers[i].get_neuron(j).get_weight(k) * hidden_layers[i - 1].get_neuron(k).get_output();
            }

            output = output + hidden_layers[i].get_neuron(j).get_bias();
            hidden_layers[i].neurons[j].set_output(hidden_activation(output));
        }
    }
    /* ----------------------------------------------------------------------------------------------------------- */

    // Output layer output from last hidden layer
    // For each neuron
    for(int i = 0; i < output_layer.get_size(); i++){
        output = 0;
        //For each weight
        for(int j = 0; j < output_layer.get_neuron(i).get_size(); j++){
            output = output + output_layer.get_neuron(i).get_weight(j) * hidden_layers[hidden_layers.size() - 1].get_neuron(j).get_output();
        }

        output = output + output_layer.get_neuron(i).get_bias();
        output_layer.neurons[i].set_output(output_activation(i, output));
    }

}

double Neural_Network::hidden_activation(double x){
    return x ? x > 0 : 0;
}

double Neural_Network::output_activation(int output_type, double x){
    if(output_type == 4){
        return ceil(10 / (1 + exp(-x)));
    } else if (output_type == 5){
        return 101 / (1 + exp(-x));
    }
    return 1 ? x > 0 : 0;
}
