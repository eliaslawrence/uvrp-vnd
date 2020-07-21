#ifndef MATRIX_H
#define MATRIX_H

#include "stop_point.h"
#include "solution_pair.h"

using namespace std;

class Matrix
{
    int rows, cols, size;
    //int *data;
    Stop_Point *data;
//    enum STOPS { NOTHING = 0, DELIVER = 1, COLLECT = 2, CHARGE = 3 }; // tipos de parada no mapa

public:
    Matrix();
    ~Matrix();
    Matrix(int r, int c);
    Matrix(int s);

    //GET e SET
    int  get_rows();
    void set_rows(int r);

    int  get_cols();
    void set_cols(int c);

    int  get_size();

    Stop_Point get_stop_point(int x, int y);
    void set_stop_point(int x, int y, int type);

    Stop_Point get_stop_point(Solution_Pair p);
    Stop_Point get_stop_point(int pos);
    void set_stop_point(int pos, int type);

    /*int  get_element(int r, int c);
    void set_element(int r, int c, int value);

    int  get_element(int pos);
    void set_element(int pos, int value);*/

    int  matrix_to_array(int r, int c);
    int array_to_matrix_x(int pos);
    int array_to_matrix_y(int pos);

    void initialize();

    void print       ();
    void save_to_file(string file_name);

    bool contains            (int reference_type);
    bool is_inside_perimeter (int x, int y);
    bool is_prohibited       (int x, int y);

    Matrix * copy();
};


#endif // MATRIX_H
