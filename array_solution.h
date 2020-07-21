#ifndef ARRAY_SOLUTION_H
#define ARRAY_SOLUTION_H

#include "solution_pair.h"
#include "route.h"
#include "neural_network.h"
#include <string>
#include <vector>

using namespace std;

class Array_Solution
{
    int            size;
//    vector<Solution_Pair> data1;
    vector<int> neighbor_visited; // Check if neighbor from heuristic was executed

    int            qty_points[4];
    int            points;
    int            time;
//    double        *z; //0: deliver/collect, 1: final charge, 2: time, 3: consumption
    vector<double> z; // pointer
    int            num_objectives;
    bool           valid    = true;
    bool           complete = false;
    int            iteration;

    bool           null = false;

//    vector<Route *> routes;    

    // NEURAL NETWORK
    vector<double> dna;

    void initialize_z(int n_obj);

public:
    vector<Route> routes;

    Array_Solution();
    ~Array_Solution();
//    Array_Solution(Array_Solution copy); // pointer
    Array_Solution(int s);
    Array_Solution(int s, int n_o);

    Array_Solution(bool n);

    //START: GET e SET
    bool is_null();

    int  get_rows();
    void set_rows(int r);

    int  get_mod();
    void set_mod(int m);

    int  get_cols();
    void set_cols(int c);

    int   get_size();
    void  set_size(int s);

    int   get_time();
    void  set_time(int t);

    int   get_qty_deliver();
    void  set_qty_deliver(int q_d);

    int   get_qty_collect();
    void  set_qty_collect(int q_c);

    int   get_qty_charge();
    void  set_qty_charge(int q_c);

    double   get_points();
//    void  set_points(int p);

    vector<double> get_z(); // pointer
    void  set_z(int p, double v);

    int   get_num_objectives();

    int  get_iteration();
    void set_iteration(int i);

    bool is_valid();
    void set_valid(bool v);

    Solution_Pair get_solution_pair(int pos);
    void          set_solution_pair(Solution_Pair s, int pos);

    void set_solution_pair(int x, int y, int pos);

    void add_solution_pair(Solution_Pair e);
    void add_solution_pair(int x, int y);

    bool is_complete();
    void set_complete(bool c);

    vector<Solution_Pair> get_data();

    void set_neighborhood_visitation(int pos, int visitation);
    int is_neighborhood_visited(int pos);

    vector<Route> get_routes(); // pointer
    void set_routes(vector<Route> routes); // pointer
    //END: GET e SET

    int  matrix_to_array(int r, int c);
    void initialize();

    void to_file(string file_name);
    void print();
    string to_string();

    string get_validation();

    Array_Solution intersection(Array_Solution a_2); // pointer
    Array_Solution copy();
    Array_Solution full_copy();
    Array_Solution invert();

    bool contains (Solution_Pair e);
    bool dominates(Array_Solution a_2); // pointer
    bool weakly_dominates(Array_Solution a_2); // pointer
    bool equals   (Array_Solution a_2); // pointer
    void resize(int s);
    void merge(Array_Solution a_2, int start); // pointer
    void merge_without_start_and_end (Array_Solution a_2, int start); // pointer
    void remove_solution_pair(int pos);
    void insert_solution_pair(Solution_Pair e, int pos);
    void insert_solution_pair(int x, int y, int pos);

    void add_route   (Route a); // pointer
    void remove_route(int pos);

    void update_values();

    // NEURAL NETWORK
    void           set_dna(Neural_Network n_n);
    void           set_dna(vector<double> dna);
    vector<double> get_dna();
};




#endif // ARRAY_SOLUTION_H
