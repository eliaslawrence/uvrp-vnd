#ifndef ROUTE_H
#define ROUTE_H

#include "solution_pair.h"
#include <string>
#include <vector>

using namespace std;

class Route
{
    int            size;
//    Solution_Pair *data;
    vector<Solution_Pair> data1;

    int            qty_points[4];
    int            points;
    int            time;
//    double        *z; //0: deliver/collect, 1: final charge, 2: time, 3: consumption

    int            num_objectives;
    bool           valid    = true;
    bool           complete = false;
    int            iteration;

    bool           null = false;

    void initialize_z(int n_obj);

public:
    vector<double> z; //pointer

    Route();
    ~Route();
//    Route(Route * copy);
//    Route(Route copy); //pointer    
    Route(int s);
    Route(int s, int n_o);

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

//    double * get_z();
    vector<double> get_z();//pointer
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
    //END: GET e SET

    int  matrix_to_array(int r, int c);
    void initialize();

    void to_file(string file_name);
    void print();
    string to_string();

    string get_validation();

//    Route * intersection(Route * a_2);
    Route intersection(Route a_2);//pointer

//    Route * copy();
//    Route * full_copy();
//    Route * invert();
    Route copy(); //pointer
    Route full_copy(); //pointer
    Route invert(); //pointer

    bool contains (Solution_Pair e);

//    bool equals   (Route * a_2);
    bool equals   (Route a_2); //pointer
    void resize(int s);
//    void merge(Route * a_2, int start);
//    void merge_without_start_and_end (Route * a_2, int start);
    void merge(Route a_2, int start); //pointer
    void merge_without_start_and_end (Route a_2, int start); //pointer
    void remove_solution_pair(int pos);
    void insert_solution_pair(Solution_Pair e, int pos);
    void insert_solution_pair(int x, int y, int pos);
};




#endif // ROUTE_H
